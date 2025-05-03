#pragma once

#include <isteamnetworkingmessages.h>
#include <isteamuser.h>
#include <isteammatchmaking.h>
#include "NetManager.hpp"
#include "../layers/LobbiesLayer.hpp"


class SteamCallbacks {
    public:
        SteamAPICall_t m_lobbyCreated;
        SteamAPICall_t m_lobbyJoined;

    private:
        STEAM_CALLBACK(SteamCallbacks, onGameJoinRequest, GameLobbyJoinRequested_t);
        STEAM_CALLBACK(SteamCallbacks, onLobbyChatUpdateWrapper, LobbyChatUpdate_t);
        STEAM_CALLBACK(SteamCallbacks, onLobbyEnter, LobbyEnter_t);
        STEAM_CALLBACK(SteamCallbacks, onNetworkingMessagesSessionRequest, SteamNetworkingMessagesSessionRequest_t);
};

