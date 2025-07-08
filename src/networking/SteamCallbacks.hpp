#pragma once

#ifdef STEAMWORKS
    #include <isteamnetworkingmessages.h>
    #include <isteamuser.h>
    #include <isteammatchmaking.h>
#else
    #include <isteamnetworklessmessages.h>
#endif

#include "NetManager.hpp"
#include "../layers/LobbiesLayer.hpp"
#include "../config.hpp"


class SteamCallbacks {
    public:
        SteamAPICall_t m_lobbyCreated;
        SteamAPICall_t m_lobbyJoined;


        #ifdef STEAMWORKS

        CCallResult< SteamCallbacks, LobbyCreated_t > m_isInLobbyCallResult;
		CCallResult< SteamCallbacks, LobbyEnter_t > m_enterLobbyCallResult;

        void onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
	    void onLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure);

        #endif STEAMWORKS

    private:
        
        #ifdef STEAMWORKS

        STEAM_CALLBACK(SteamCallbacks, onGameJoinRequest, GameLobbyJoinRequested_t);
        STEAM_CALLBACK(SteamCallbacks, onLobbyChatUpdateWrapper, LobbyChatUpdate_t);
        STEAM_CALLBACK(SteamCallbacks, onLobbyEnter, LobbyEnter_t);

        #endif STEAMWORKS


        STEAM_CALLBACK(SteamCallbacks, onNetworkingMessagesSessionRequest, SteamNetworkingMessagesSessionRequest_t);
};

