#pragma once

#include "NetworkingHeaders.hpp"

class NetManager {
    public:
        static NetManager* get();

        // TODO: Add params
        // Queues data. This will be sent automatically in its update() function.
        static void queueData();

        void update();


        // Called to enter the level editor
	    void enterLevelEditor();
        
        // PARAMS!!
        void joinSteamLobby();
        void leaveSteamLobby();

        // Uses port in config.hpp
        void joinIPLobby(const char* address);

        void fetchMemberList();

        bool m_isRequestingLevelString = false;

		// m_options can be deconstructed for default values.
		struct lobbyOptions m_options;

        
    private:
        void flushQueue();
        // Receive data and then pars it
        void receiveData();
        // Parse data. Called in receiveData.
        Result<int> parseData();

        uint64 m_lobbyId;
		CSteamID m_hostID;
        bool m_isHost = false;
        bool m_isInLobby = false;
        std::vector<SteamNetworkingIdentity> m_playersInLobby;
        std::vector<CSteamID> m_excludedMemberList;
        
}