#include <Geode/Geode.hpp>

#include "../../config.hpp"
#include "CursorManager.hpp"
#include "Geode/loader/Log.hpp"
#include "NetManager.hpp"
#include "ccserialization_generated.h"
#include "ctcursor_generated.h"
#include "../../hooks/ModifyEditorLayer.hpp"
#include "../../utils/Utills.hpp"

#ifdef NO_STEAMWORKS
    #include <debug/steamnetworkingsockets.h>
    #include <debug/isteamnetworkingutils.h>
    #include "../debug/client.hpp"
#endif

using namespace geode::prelude;


CursorManager* CursorManager::get() {
    return NetManager::get()->m_cursorManager;
}

CreateTogetherCursor* CursorManager::getPlayerCursor(SteamNetworkingIdentity id) {
    return CursorManager::get()->m_playerCursors.at(getCursorHash(id));
}


void CursorManager::sendCursorUpdateToAll() {
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());

    if (!level) {
        log::error("Could not find level?");
        return;
    }

    auto ccpos = level->m_objectLayer->getPosition() + getMousePos();
    auto pos = CTSerialize::CCPos();
    auto cursorUpdate = CTSerialize::cursor::CreateCursorUpdate(this->m_cursorBuilder, &pos, CTSerialize::cursor::StatusType_None);
    this->sendMessage(cursorUpdate);
    this->m_cursorBuilder.Clear();
}

void CursorManager::updateCursorPositon(CreateTogetherCursor* cursor, CCPoint position) {
    cursor->setPosition(position);
}

void CursorManager::update() {
    receiveCursorData();

    auto netManager = NetManager::get();

    if (!netManager->m_ignoreMessages && netManager->m_isInLobby) {
        this->sendCursorUpdateToAll();
    }

}

void CursorManager::cursorNetworkingPrelude() {

    log::info("Setting up CursorManager.");

    #ifdef NO_STEAMWORKS
        SteamNetworkingIPAddr serverAddr;
        serverAddr.SetIPv4(0x7f000001, DEDICATED_CURSOR_PORT);
        char szAddr[ SteamNetworkingIPAddr::k_cchMaxString ];
        serverAddr.ToString(szAddr, sizeof(szAddr), true);
        log::info("Connecting to cursor server at {}", szAddr);

        SteamNetworkingConfigValue_t opt;
        // todo: use a unique connectionStatus
        opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)steamNetConnectionStatusChangedCallback);
        this->m_cursorConnection = SteamNetworkingSockets()->ConnectByIPAddress(serverAddr, 1, &opt);
        if (this->m_cursorConnection == k_HSteamNetConnection_Invalid) {
            log::warn("Could not create a connection with the cursor server");
        }

    #endif

    // TODO: FINISH
}

void CursorManager::receiveCursorData() {
    
    SteamNetworkingMessage_t* messageList[MAX_CURSOR_MESSAGES];

    #ifdef STEAMWORKS
        auto numMessages = SteamNetworkingMessages()->ReceiveMessagesOnChannel(CURSOR_CHANNEL, messageList, MAX_CURSOR_MESSAGES);
    #else
        auto numMessages = SteamNetworkingSockets()->ReceiveMessagesOnConnection(this->m_cursorConnection, messageList, MAX_CURSOR_MESSAGES);
    #endif

    if (numMessages < 0) {
        log::warn("CursorManager::receiveData(): Error receiving messages {}", numMessages);
    }

    for (int i = 0; i < numMessages; i++) {
        
        SteamNetworkingMessage_t* msg = messageList[i];

        // Uhh idk anymore what this is
        // This should create a msg->GetSize() sized data object.
        uint8_t* data = new uint8_t[msg->GetSize()];
        memcpy(data, msg->GetData(), msg->GetSize());
        
        // data = static_cast<const uint8_t*>(message->GetData());
        
        auto messageHeader = CTSerialize::cursor::GetCursorUpdate(data);
        flatbuffers::Verifier verifier(data, msg->GetSize());
        bool isVerified = CTSerialize::cursor::VerifyCursorUpdateBuffer(verifier);

        if (!isVerified) {
            log::warn("Failed to verify cursor message");
            return;
        }
        
        auto out = this->parseCursorData(messageHeader, msg->m_identityPeer);

        // auto s = flatbuffers::FlatBufferToString(data, CTSerialize::MessageHeaderTypeTable());
        // log::debug("RecvMessage:{}", s);


        if (!out) {
            log::warn("Something went wrong while parsing cursor message: {}", out.unwrapErr());
        }

        // TODO: Call release earlier
        msg->Release();
        delete[] data;
    }
}

Result<uint8_t> CursorManager::parseCursorData(const CTSerialize::cursor::CursorUpdate* msg, SteamNetworkingIdentity msgSource) {
        
    if (auto msgPos = msg->position()) {
        cocos2d::CCPoint pos = {msgPos->x(), msgPos->y()};
        updateCursorPositon(this->m_playerCursors.at(getCursorHash(msgSource)), pos);
    }
    else {
        return Err("parseCursorData: no position in message");
    }
    
    return Ok(0);
}

void CursorManager::sendMessage(flatbuffers::Offset<CTSerialize::cursor::CursorUpdate> out) {
    this->m_cursorBuilder.Finish(out);
    auto netManager = NetManager::get();

    // auto s = flatbuffers::FlatBufferToString(this->m_builder.GetBufferPointer(), CTSerialize::MessageHeaderTypeTable());
    // log::debug("SentMessage:{}", s);

    #ifdef STEAMWORKS
        for (auto const& member : netManager->m_playersInLobby) {
            SteamNetworkingMessages()->SendMessageToUser(member, this->m_cursorConnection, this->m_cursorBuilder.GetBufferPointer(), this->m_cursorBuilder.GetSize(), k_nSteamNetworkingSend_UnreliableNoDelay, CURSOR_CHANNEL);
        }
    #else
        SteamNetworkingSockets()->SendMessageToConnection(this->m_cursorConnection, this->m_cursorBuilder.GetBufferPointer(), this->m_cursorBuilder.GetSize(), k_nSteamNetworkingSend_UnreliableNoDelay, nullptr);
    #endif
}