#pragma once

#include "NetworkingHeaders.hpp"
#include "../types/LobbyData.hpp"
#include <isteammatchmaking.h>

// TODO: Remove this.
#ifdef USE_TEST_SERVER
	#include <WinSock2.h>
	#include "../types/PlaceboMsg.hpp"
#endif


class NetManager {
    public:
        static NetManager* get();

        // TODO: Add params
        // Queues data. This will be sent automatically in its update() function.
        static void queueData();

        void update();


        // Called to enter the level editor
	    void enterLevelEditor();
        
        void joinSteamLobby(CSteamID steamIDLobby);
        void leaveCurrentSteamLobby();

        // Uses port in config.hpp
        void joinIPLobby(const char* address);

        void fetchMemberList();

        bool m_isRequestingLevelString = false;
        bool m_isInEditorLayer = false;
        uint64 m_lobbyId;
        bool m_isHost = false;
        CSteamID m_hostID;
        bool m_isInLobby = false;



		// m_options can be deconstructed for default values.
		struct lobbyOptions m_options;

        SOCKET m_socket;
        

        
    private:
        void preFlushProcess();
        // Send data and empty queue
        void flushQueue();
        // Receive data and then parse it
        void receiveData();
        // Parse data. Called in receiveData.
        Result<int> parseData();

        std::vector<SteamNetworkingIdentity> m_playersInLobby;
        std::vector<CSteamID> m_excludedMemberList;
        
};