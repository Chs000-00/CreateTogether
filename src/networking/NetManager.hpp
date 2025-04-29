#pragma once


class NetManager {
    public:
        static NetManager* get();

        // TODO: Add params
        // Queues data. This will be sent automatically in its update() function.
        static NetManager* queueData();

        void update();


        // Called to enter the level editor
	    void enterLevelEditor();
        
        void enterSteamLobby();
        void leaveSteamLobby();

        // Uses port in config.hpp
        void enterIPLobby(const char* address);

        void fetchMemberList();

        bool m_isRequestingLevelString = false;

		// m_options can be deconstructed for def values.
		struct lobbyOptions m_options;

        
    private:
        void flushQueue();
        void reciveAndParse();
        Result<int> parseData();

        uint64 m_lobbyId;
		CSteamID m_hostID;
        bool m_isHost = false;
        bool m_isInLobby = false;
        std::vector<SteamNetworkingIdentity> m_playersInLobby;
        
}