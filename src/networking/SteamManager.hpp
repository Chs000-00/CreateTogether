#pragma once

#ifdef STEAMWORKS
    #include <isteamnetworkingmessages.h>
    #include <isteamuser.h>
    #include <isteammatchmaking.h>
#else
    #include <debug/isteamnetworkingmessages.h>
#endif

#include "NetManager.hpp"
#include "../layers/LobbiesLayer.hpp"
#include "../config.hpp"

#include "../utils/Singleton.hpp"



class SteamManager: public Singleton<SteamManager> {
    public:
        SteamAPICall_t m_lobbyCreated;
        SteamAPICall_t m_lobbyJoined;


        #ifdef STEAMWORKS

        CCallResult< SteamManager, LobbyCreated_t > m_isInLobbyCallResult;
        CCallResult< SteamManager, LobbyEnter_t > m_enterLobbyCallResult;

        void onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
        void onLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure);

        #else

        SteamManager();
        void initSockets();

        #endif

    private:
        
        #ifdef STEAMWORKS

        STEAM_CALLBACK(SteamManager, onGameJoinRequest, GameLobbyJoinRequested_t);
        STEAM_CALLBACK(SteamManager, onLobbyChatUpdateWrapper, LobbyChatUpdate_t);
        STEAM_CALLBACK(SteamManager, onLobbyEnter, LobbyEnter_t);
        STEAM_CALLBACK(SteamManager, onNetworkingMessagesSessionRequest, SteamNetworkingMessagesSessionRequest_t);
        
        #endif
};

