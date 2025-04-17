#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <isteammatchmaking.h>
#include <isteamfriends.h>
#include <steam_api_common.h>
#include <steamnetworkingtypes.h>
#include <isteamnetworkingmessages.h>
#include "../layers/LobbiesLayer.hpp"
#include "../config.hpp"

#ifdef USE_TEST_SERVER
	#include <WinSock2.h>
	#include "../types/PlaceboMsg.hpp"
#endif

using namespace geode::prelude;

class CallbackManager {
private:
	STEAM_CALLBACK(CallbackManager, onGameJoinRequest, GameLobbyJoinRequested_t);
	STEAM_CALLBACK(CallbackManager, onLobbyChatUpdateWrapper, LobbyChatUpdate_t);
	STEAM_CALLBACK(CallbackManager, onLobbyEnter, LobbyEnter_t);
	STEAM_CALLBACK(CallbackManager, onNetworkingMessagesSessionRequest, SteamNetworkingMessagesSessionRequest_t);
};

class $modify(MyGameManager, GameManager) {
    struct Fields {
		//  Inner Structs?
        SteamAPICall_t m_lobbyCreated;
        SteamAPICall_t m_lobbyJoined;
        uint64 m_lobbyId;
		CSteamID m_hostID;
        bool m_isInLobby = false;
		bool m_isHost = false; // TODO: use GetLobbyOwner instead? Possibly?
		// I dont know what this does
		bool m_isInEditorLayer = false;
		bool m_isRequestingLevelString = false;
		
		// Don't remember why this is a seperate option. Most likley to destruct state when leaving the lobby.
		struct lobbyOptions m_options;

		struct SharedMassEdit {
			// TODO: Move this stuff to some other class
			matjson::Value m_moveList;
			bool m_sendMoveList;

			matjson::Value m_groupIDEdits;
			bool m_sendGroupIDEdits;
			bool m_isAddingGroupID;
			int m_groupIDToEdit;
		} m_sharedMassEdit;

	    	// LobbiesLayer* m_lobbyLayer = nullptr;
		CallbackManager m_callbackManager;

		std::vector<SteamNetworkingIdentity> m_playersInLobby;

        #ifdef USE_TEST_SERVER
			SOCKET m_socket;
        #endif

		CCallResult< MyGameManager, LobbyCreated_t > m_isInLobbyCallResult;
		CCallResult< MyGameManager, LobbyEnter_t > m_enterLobbyCallResult;
    };

	void update(float p0);
    void onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
	void onLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure);

	// Called to enter the level editor
	void enterLevelEditor();

	// Send data to a specific user, prob broken
	void sendDataToUser(SteamNetworkingIdentity usr, const char* out);

	// Update the memberList
	void fetchMemberList();

	// Send data to each member. ReceiveData is some weird useless bool that I should prob remove
	void sendDataToMembers(std::string data, bool receiveData = false);

	// Receive data
	void receiveData();

	// Parse recived data
	Result<int> parseDataReceived(matjson::Value data, NETWORKING_MSG* msg);

	// Send data each frame cause idfk why not
	void lateSendData();

	// Leave the current lobby if it exists. Otherwise do nothing
	void leaveLobby();
	
	// Return a matjson::Value from LevelEditorLayer::get()->getLevelString()
	matjson::Value getLevelStringMatjson();

	// This is just one fucking function call why do I even have this
	static bool validateData(matjson::Value data);

};
