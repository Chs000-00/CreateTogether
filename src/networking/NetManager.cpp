#include <Geode/Geode.hpp>
#include "../hooks/ModifyGameManager.hpp"
#include "NetManager.hpp"
#include "./highlevel/NetSend.hpp"
#include "../utils/Utills.hpp"
#include "highlevel/NetRecv.hpp" // IWYU pragma: keep
#include <flatbuffers/minireflect.h>
#include <ctserialize_generated.h>
#include "../ui/WaitingForHostPopup.hpp"

using namespace geode::prelude;

// TODO: Try compiling ValveSoftware/GameNetworkingSockets:partner to possibly fix this ugly crap?
#ifdef NO_STEAMWORKS
    #include <debug/steamnetworkingsockets.h>
    #include <debug/isteamnetworkingutils.h>
    #include "debug/isteamnetworkingsockets.h"
    #include "../debug/client.hpp"
#endif


// Why the f*** can't c++ inline this f***ing shit properly without some stupid use of header guards
NetManager* NetManager::get() {
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    return gameManager->m_fields->m_netManager;
}

bool NetManager::getIsInLobby() {
    return NetManager::get()->m_isInLobby;
}

bool NetManager::getWasDataSent() {
    return NetManager::get()->m_wasDataSent;
}


void NetManager::update() {

    #ifdef STEAMWORKS
        SteamAPI_RunCallbacks();
    #else
        SteamNetworkingSockets()->RunCallbacks();
    #endif

    if (this->m_isInLobby) {
        this->receiveData();
        // this->sendQueuedData();
        this->m_cursorManager->update();
    }
}

void NetManager::leaveLobby() {
    

    log::info("ISINLOBBY! {}", this->m_isInLobby);

    if (this->m_isInLobby) {

        m_isInLobby = false;

        #ifdef STEAMWORKS

            log::info("Leaving lobby with ID {}", this->m_lobbyId);
            SteamMatchmaking()->LeaveLobby(this->m_lobbyId);
            this->m_lobbyId = 0;

        #else

            log::info("Leaving debug lobby");
            
        #endif

        // this->m_lobbyCreated = 0;
        // this->m_lobbyJoined = 0;

    }
    else {
        log::info("Can't leave lobby because not in lobby!");
    }

    // this->m_isInEditorLayer = false;

    // Clear buffer so it dosen't cause issues later.
    this->m_builder.Clear();
    
}

void NetManager::sendMessageToUser(SteamNetworkingIdentity usr, flatbuffers::Offset<CTSerialize::MessageHeader> out) {
    this->m_builder.Finish(out);

    auto s = flatbuffers::FlatBufferToString(this->m_builder.GetBufferPointer(), CTSerialize::MessageHeaderTypeTable());
    log::debug("SentMessage:{}", s);

    #ifdef STEAMWORKS
        SteamNetworkingMessages()->SendMessageToUser(usr, this->m_builder.GetBufferPointer(), this->m_builder.GetSize(), k_nSteamNetworkingSend_Reliable, EDITOR_CHANNEL);
    #else
        SteamNetworkingSockets()->SendMessageToConnection(this->m_connection, this->m_builder.GetBufferPointer(), this->m_builder.GetSize(), k_nSteamNetworkingSend_Reliable, nullptr);
    #endif
}

void NetManager::sendMessage(flatbuffers::Offset<CTSerialize::MessageHeader> out) {
    this->m_builder.Finish(out);

    auto s = flatbuffers::FlatBufferToString(this->m_builder.GetBufferPointer(), CTSerialize::MessageHeaderTypeTable());
    log::debug("SentMessage:{}", s);

    #ifdef STEAMWORKS
        for (auto const& member : this->m_playersInLobby) {
            SteamNetworkingMessages()->SendMessageToUser(member, this->m_builder.GetBufferPointer(), this->m_builder.GetSize(), k_nSteamNetworkingSend_Reliable, EDITOR_CHANNEL);
        }
    #else
        SteamNetworkingSockets()->SendMessageToConnection(this->m_connection, this->m_builder.GetBufferPointer(), this->m_builder.GetSize(), k_nSteamNetworkingSend_Reliable, nullptr);
    #endif
}
 

void NetManager::enterLevelEditorPrelude() {

    log::info("Enter Level Editor Called.");

    // Workaround to fix movement stuff
    this->m_wasDataSent = true;

    WaitingForHostPopup::create();
    this->m_isRequestingLevelString = true;

    #ifdef NO_STEAMWORKS
        SteamNetworkingIPAddr serverAddr;
        serverAddr.SetIPv4(0x7f000001, DEDICATED_EDITOR_PORT);
        char szAddr[ SteamNetworkingIPAddr::k_cchMaxString ];
        serverAddr.ToString(szAddr, sizeof(szAddr), true);
        log::info("Connecting to editor server at {}", szAddr);

        SteamNetworkingConfigValue_t opt;
        opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)steamNetConnectionStatusChangedCallback);
        this->m_connection = SteamNetworkingSockets()->ConnectByIPAddress(serverAddr, 1, &opt);
        if (this->m_connection == k_HSteamNetConnection_Invalid) {
            log::warn("Could not create a connection with the editor server");
        }

    #endif

    this->m_isInLobby = true;

    this->fetchMemberList();

    this->m_cursorManager->cursorNetworkingPrelude();

    SteamNetworkingIdentity host;

    host.SetSteamID(this->m_hostID);

    sendRequestLevel();
}


void NetManager::fetchMemberList() {

    #ifdef STEAMWORKS
        int lobbyMemberCount = SteamMatchmaking()->GetNumLobbyMembers(this->m_lobbyId);
        SteamNetworkingIdentity member;

        log::debug("FetchMemberList Called; Fetching {} users:", lobbyMemberCount);
        this->m_playersInLobby.clear();

        for (int i = 0; i < lobbyMemberCount; i++) {
            CSteamID steamIDLobbyMember = SteamMatchmaking()->GetLobbyMemberByIndex(this->m_lobbyId, i);
            if (steamIDLobbyMember == SteamUser()->GetSteamID()) {
                log::debug("(Self) - {} with SteamID: {}", SteamFriends()->GetPersonaName(), steamIDLobbyMember.ConvertToUint64());
                continue;
            }

            member.SetSteamID(steamIDLobbyMember);
            log::debug("{} with SteamID: {}", SteamFriends()->GetFriendPersonaName(steamIDLobbyMember), steamIDLobbyMember.ConvertToUint64());
            this->m_playersInLobby.push_back(member);   
        } 
    #else

    log::warn("fetchMemberList without STEAMWORKS. Ignoring...");
    
    // SteamNetworkingIdentity server;

    // server.SetIPv4Addr(0x7f000001, DEDICATED_PORT);

    // this->m_playersInLobby.push_back(server);

    #endif

}

void NetManager::receiveData() {

    // if (this->m_isInEditorLayer) {
    // 	return;
    // }

    SteamNetworkingMessage_t* messageList[MAX_EDITOR_MESSAGES];

    #ifdef STEAMWORKS
        auto numMessages = SteamNetworkingMessages()->ReceiveMessagesOnChannel(EDITOR_CHANNEL, messageList, MAX_EDITOR_MESSAGES);
    #else
        auto numMessages = SteamNetworkingSockets()->ReceiveMessagesOnConnection(this->m_connection, messageList, MAX_EDITOR_MESSAGES);
    #endif

    if (numMessages < 0) {
        log::warn("NetManager::receiveData(): Error receiving messages");
    }

    for (int i = 0; i < numMessages; i++) {
        
        SteamNetworkingMessage_t* msg = messageList[i];

        // Uhh idk anymore what this is
        // This should create a msg->GetSize() sized data object.
        uint8_t* data = new uint8_t[msg->GetSize()];
        memcpy(data, msg->GetData(), msg->GetSize());
        
        // data = static_cast<const uint8_t*>(message->GetData());
        
        auto messageHeader = CTSerialize::GetMessageHeader(data);
        flatbuffers::Verifier verifier(data, msg->GetSize());
        bool isVerified = CTSerialize::VerifyMessageHeaderBuffer(verifier);

        if (!isVerified) {
            log::warn("Failed to verify message");
            return;
        }

        auto s = flatbuffers::FlatBufferToString(data, CTSerialize::MessageHeaderTypeTable());
        log::debug("RecvMessage:{}", s);
        
        auto out = this->parseData(messageHeader, msg->m_identityPeer);


        if (!out) {
            log::warn("Something went wrong while parsing: {}", out.unwrapErr());
        }

        // TODO: Call release earlier
        msg->Release();
        delete[] data;
    }
}

Result<uint8_t> NetManager::parseData(const CTSerialize::MessageHeader* msg, SteamNetworkingIdentity msgSource) {
    auto dmsg = msg->body();
    switch (msg->body_type()) {
        case CTSerialize::MessageBody_CreateObjects: {
            SERIALIZE_AND_RECEIVE(CreateObjects);
            break;
        }

        case CTSerialize::MessageBody_DeleteObjects: {
            SERIALIZE_AND_RECEIVE(DeleteObjects);
            break;
        }

        case CTSerialize::MessageBody_MoveObjects: {
            SERIALIZE_AND_RECEIVE(MoveObjects);
            break;
        }

        case CTSerialize::MessageBody_LevelSettingChange: {
            SERIALIZE_AND_RECEIVE(LevelSettingChange);
            break;
        }

        case CTSerialize::MessageBody_RotateObjects: {
            SERIALIZE_AND_RECEIVE(RotateObjects);
            break;
        }

        case CTSerialize::MessageBody_PasteObjects: {
            SERIALIZE_AND_RECEIVE(PasteObjects);
            break;
        }

        case CTSerialize::MessageBody_ModifyObjects: {
            SERIALIZE_AND_RECEIVE(ModifyObjects);
            break;
        }

        case CTSerialize::MessageBody_ChangeDefaultColor: {
            SERIALIZE_AND_RECEIVE(ChangeDefaultColor);
            break;
        }

        case CTSerialize::MessageBody_RequestLevel: {
            SERIALIZE_AND_RECEIVE(RequestLevel, msgSource);
            break;
        }

        case CTSerialize::MessageBody_ReturnLevelString: {
            SERIALIZE_AND_RECEIVE(ReturnLevelString, msgSource);
            break;
        }

        case CTSerialize::MessageBody_UpdateSong: {
            SERIALIZE_AND_RECEIVE(UpdateSong);
            break;
        }

        case CTSerialize::MessageBody_ChangeArt: {
            SERIALIZE_AND_RECEIVE(ChangeArt);
            break;
        }

        case CTSerialize::MessageBody_SpeedChange: {
            SERIALIZE_AND_RECEIVE(SpeedChange);
            break;
        }

        case CTSerialize::MessageBody_GameModeChange: {
            SERIALIZE_AND_RECEIVE(GameModeChange);
            break;
        }

        case CTSerialize::MessageBody_PlayerCursorData: {
            SERIALIZE_AND_RECEIVE(PlayerCursorData, msgSource);
            break;
        }

        case CTSerialize::MessageBody_RequestForCursors: {
            SERIALIZE_AND_RECEIVE(RequestForCursors);
            break;
        }

        default:
            return Err("Invalid Union Type");
    }
    
    return Ok(0);
}

#ifdef STEAMWORKS
void NetManager::joinSteamLobby(GameLobbyJoinRequested_t* lobbyInfo) {
    // TODO: Remove this
    log::debug("JoinLobbyRequest Called with steamID: {} | friendID: {} | friendName: {}", lobbyInfo->m_steamIDLobby.ConvertToUint64(), lobbyInfo->m_steamIDFriend.ConvertToUint64(), SteamFriends()->GetFriendPersonaName(lobbyInfo->m_steamIDFriend));
    this->m_isInLobby = SteamMatchmaking()->JoinLobby(lobbyInfo->m_steamIDLobby); // I'm not sure if this is needed
}
#endif
