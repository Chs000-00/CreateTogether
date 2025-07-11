#pragma once

#ifdef STEAMWORKS
    #include <isteamnetworkingmessages.h>
    #include <isteamuser.h>
    #include <isteammatchmaking.h>
#else
    #include <debug/isteamnetworkingmessages.h>
#endif


#include <flatbuffers/flatbuffers.h>
#include <ctserialize_generated.h>

#include "../types/LobbyData.hpp"
#include "../ui/WaitingForHostPopup.hpp"

class NetManager {
    public:
        static NetManager* get();

        static bool getIsInLobby();

        // Called each tick or smth idk how this shit works
        void update();

        // Called to enter the level editor
	    void enterLevelEditor();
        
        // Joins a steam lobby with a specified ID
        #ifdef USE_STEAMWORKS
        void joinSteamLobby(GameLobbyJoinRequested_t* lobbyInfo);
        #endif

        // Leaves the current steam lobby
        void leaveLobby();

        // Uses port in config.hpp
        void joinIPLobby(const char* address);

        // Fetches and updates a member list. Should be called at anytime a user leaves/joins, or otherwise gets kicked.
        void fetchMemberList();

        // Should the user update the level to the current level string? Might have a better implementation later.
        bool m_isRequestingLevelString = false;

        // Is the user currently in the editor layer? Yeah idk what this does.
        bool m_isInEditorLayer = false;

        // Steam LobbyID
        uint64 m_lobbyId;
        
        // Is currently hosting? This shouldn't be relied upon, use the steamworks function instead in some cases.
        bool m_isHost = false;

        // Who is the host? Used to check if messages come from the host.
        CSteamID m_hostID;

        // Are you in an editing lobby
        bool m_isInLobby = false;

		// m_options can be deconstructed for default values. Idk why I made this this way
        // Lobby options, such is if the server is private or not.
		struct lobbyOptions m_options;

        // A list of players in the lobby. sendMessage() sends the data to all of these users, and fetchMemberList() updates it.
        std::vector<SteamNetworkingIdentity> m_playersInLobby;

        // Flatbuffer builder for building flatbuffers, or something idk
        flatbuffers::FlatBufferBuilder m_builder;
        
        std::vector<uint8_t> m_bodyType;
        std::vector<flatbuffers::Offset<void>> m_body;


        #ifdef NO_STEAMWORKS
            HSteamNetConnection connection;
        #endif

        
    private:
        // Sends all the currently queued data. Oh and also deletes the data afterward.
        void sendQueuedData();

        // Receive data and then parse it
        void receiveData();
        // Parse data. Called in receiveData.
        Result<uint8_t> parseData(const CTSerialize::MessageHeader* msg);

        // Steamnetworking has no way to kick/ban users. I think? Instead just remove the dudes access to m_playersInLobby and any
        // Further attempts to join (this member is specificaly for fetchMemberList())
        // None of this has timeouts cause I am too lazy to implement them so idk
        std::vector<CSteamID> m_excludedMemberList;

        // TODO: Check EResult

        // Sends an entire message to everyone in the lobby
        void sendMessage(flatbuffers::Offset<CTSerialize::MessageHeader> out);

        // Sends an entire message to a single person
        void sendMessageToUser(SteamNetworkingIdentity usr, flatbuffers::Offset<CTSerialize::MessageHeader> out);

};