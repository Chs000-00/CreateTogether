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
        SteamAPICall_t m_lobbyCreated;
        SteamAPICall_t m_lobbyJoined;
        uint64 m_lobbyId;
		CSteamID m_hostID;
        bool m_isInLobby = false;
		bool m_isHost = false; // TODO: use GetLobbyOwner instead? Possibly?
		bool m_isInEditorLayer = false;

		LevelEditorLayer* m_level;
    	LobbiesLayer* m_lobbyLayer = nullptr;
		CallbackManager m_callbackManager;

		std::vector<SteamNetworkingIdentity> m_playersInLobby;

		CCallResult< MyGameManager, LobbyCreated_t > m_isInLobbyCallResult;
		CCallResult< MyGameManager, LobbyEnter_t > m_enterLobbyCallResult;
		CCallResult< MyGameManager, LobbyMatchList_t  > m_lobbyMatchListCallResult;

    };

	void update(float p0);
    void onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
	void onLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure);
	void onLobbyMatchList(LobbyMatchList_t *pLobbyMatchList, bool bIOFailure);
	void fetchMemberList();
	void sendDataToMembers(const char* data, bool receiveData = false);
	void receiveData();
	void leaveLobby();
	matjson::Value MyGameManager::getLevelStringMatjson();

	static bool validateData(matjson::Value data);

};
