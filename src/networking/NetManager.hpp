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

        // TODO: EResult? Vectors? Idk.
        // Queues data. This will be sent automatically in its update() function.
        static void queueData(flatbuffers::FlatBufferBuilder out);
        void sendData(flatbuffers::FlatBufferBuilder* out);
        void sendMessageHeaderToUser(SteamNetworkingIdentity usr, flatbuffers::FlatBufferBuilder* out);

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

        struct MassEdit {

			matjson::Value m_groupIDEdits;
			bool m_sendGroupIDEdits;
			bool m_isAddingGroupID;
			int m_groupIDToEdit;

		} m_massEdit;


		// m_options can be deconstructed for default values.
		struct lobbyOptions m_options;

        SOCKET m_socket;

        std::vector<SteamNetworkingIdentity> m_playersInLobby;

        

        
    private:
        void preFlushProcess();
        // Send data and empty queue
        void flushQueue();
        // Receive data and then parse it
        void receiveData();
        // Parse data. Called in receiveData.
        Result<int> parseData();

        std::vector<CSteamID> m_excludedMemberList;
        
};