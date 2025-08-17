#include "CursorManager.hpp"
#include "NetManager.hpp"
#include "ccserialization_generated.h"
#include "ctcursor_generated.h"
#include "../../hooks/ModifyEditorLayer.hpp"

#ifdef NO_STEAMWORKS
    #include <debug/steamnetworkingsockets.h>
    #include <debug/isteamnetworkingutils.h>
    #include "../debug/client.hpp"
#endif

CursorManager* CursorManager::get() {
    return NetManager::get()->m_cursorManager;
}

std::vector<CreateTogetherCursor*> CursorManager::getPlayerCursors() {
    return CursorManager::get()->m_playerCursors;
}


void CursorManager::sendCursorUpdateToAll() {
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());

    if (!level) {
        return;
    }

    auto pos = CTSerialize::CCPos();
    auto cursorUpdate = CTSerialize::cursor::CreateCursorUpdate(this->m_cursorBuilder, &pos, CTSerialize::cursor::StatusType_None);
    this->sendMessage(cursorUpdate);
    this->m_cursorBuilder.Clear();
}

void CursorManager::displayCursorAtPositon(CreateTogetherCursor* cursor, CCPoint position) {
        
}

void CursorManager::cursorNetworkingPrelude() {
    #ifdef NO_STEAMWORKS
        SteamNetworkingIPAddr serverAddr;
        serverAddr.SetIPv4(0x7f000001, DEDICATED_EDITOR_PORT);
        char szAddr[ SteamNetworkingIPAddr::k_cchMaxString ];
        serverAddr.ToString(szAddr, sizeof(szAddr), true);
        log::info("Connecting to {}", szAddr);

        SteamNetworkingConfigValue_t opt;
        opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)steamNetConnectionStatusChangedCallback);
        this->cursorConnection = SteamNetworkingSockets()->ConnectByIPAddress(serverAddr, 1, &opt);
        if (this->cursorConnection == k_HSteamNetConnection_Invalid) {
            log::warn("Could not create a connection");
        }

    #endif
}

void CursorManager::sendMessage(flatbuffers::Offset<CTSerialize::cursor::CursorUpdate> out) {
    this->m_cursorBuilder.Finish(out);
    auto netManager = NetManager::get();

    // auto s = flatbuffers::FlatBufferToString(this->m_builder.GetBufferPointer(), CTSerialize::MessageHeaderTypeTable());
    // log::debug("SentMessage:{}", s);

    #ifdef STEAMWORKS
        for (auto const& member : netManager->m_playersInLobby) {
            SteamNetworkingMessages()->SendMessageToUser(member, netManager->connection, this->m_cursorBuilder.GetBufferPointer(), this->m_cursorBuilder.GetSize(), k_nSteamNetworkingSend_UnreliableNoDelay, CURSOR_CHANNEL);
        }
    #else
        SteamNetworkingSockets()->SendMessageToConnection(netManager->connection, this->m_cursorBuilder.GetBufferPointer(), this->m_cursorBuilder.GetSize(), k_nSteamNetworkingSend_UnreliableNoDelay, nullptr);
    #endif
}