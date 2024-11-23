#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <isteammatchmaking.h>
#include <isteamfriends.h>
#include <steam_api_common.h>
#include <steamnetworkingtypes.h>

using namespace geode::prelude;

class CallbackManager {
private:
	STEAM_CALLBACK(CallbackManager, onGameJoinRequestWrapper, GameLobbyJoinRequested_t);
	STEAM_CALLBACK(CallbackManager, onLobbyDataUpdateWrapper, LobbyDataUpdate_t);
};

class $modify(MyGameManager, GameManager) {

    struct Fields {
        SteamAPICall_t m_lobby = NULL;
        uint64 m_lobbyId = 0; // TODO: I probably shouldn't be using 0 as the lobbyID!
        bool m_isInLobby = false;
		bool m_isHost = false; // TODO: use GetLobbyOwner instead? Possibly?
		bool m_isInEditorLayer = false;
		LevelEditorLayer* m_level;
		CallbackManager callbackManager;

		std::vector<SteamNetworkingIdentity> m_playersInLobby;

		CCallResult< MyGameManager, LobbyCreated_t > m_isInLobbyCallResult;
		CCallResult< MyGameManager, LobbyEnter_t > m_enterLobbyCallResult;
    };

	void update(float p0);
    void onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
	void onLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure);
	void onGameJoinRequest(GameLobbyJoinRequested_t* pCallback);
	void joinLobbyFromRequest(GameLobbyJoinRequested_t* pCallback);
	void fetchMemberList();
	void sendDataToMembers(const char* data);
	void receiveData();
};
