#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <isteammatchmaking.h>
#include <isteamnetworkingmessages.h>
#include "ModifyGameManager.hpp"


using namespace geode::prelude;

void CallbackManager::onGameJoinRequestWrapper(GameLobbyJoinRequested_t* pCallback) {
	auto gameManager = static_cast<MyGameManager*>(GameManager::get());
	gameManager->onGameJoinRequest(pCallback);
}

void CallbackManager::onLobbyChatUpdateWrapper(LobbyChatUpdate_t* pCallback) {
	auto gameManager = static_cast<MyGameManager*>(GameManager::get());
	gameManager->fetchMemberList();
}

void MyGameManager::onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure) {
	if (pCallback->m_eResult == k_EResultOK) {
		log::info("Created Lobby!");
		m_fields->m_isInLobby = true;
		m_fields->m_isHost = true;
		m_fields->m_lobbyId = pCallback->m_ulSteamIDLobby;

		// Constants can be changed in CMakeLists.txt
		// Kind of a bad idea but who cares

		// SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "lobby_type", MOD_LOBBY_NAME); // TODO: Uncomment this
		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "version", MOD_VERSION);
	}
}

void MyGameManager::onLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure) {
	m_fields->m_lobbyId = pCallback->m_ulSteamIDLobby;
	m_fields->m_level = LevelEditorLayer::create(GJGameLevel::create(), false);
	log::info("EnterLobby called!");
	if (m_fields->m_isHost) {
		log::info("Entered Lobby as host, not doing anything anymore!");
		return;
	}
	else {
		switchToScene(m_fields->m_level);
	}
}

// I am slowly devolving into madness
void MyGameManager::onGameJoinRequest(GameLobbyJoinRequested_t* pCallback) {
	geode::createQuickPopup(
		"Lobby?",            // title
		"Join Lobby?",   // content
		"Nah", "Yeah",      // buttons
		[this, pCallback](auto, bool btn2) {
			if (btn2) {
				this->joinLobbyFromRequest(pCallback);
			}
		}
	);
}

void MyGameManager::joinLobbyFromRequest(GameLobbyJoinRequested_t* pCallback) {
	log::debug("Joining game with steamID: {}", pCallback->m_steamIDLobby.ConvertToUint64());
	this->m_fields->m_lobby = SteamMatchmaking()->JoinLobby(pCallback->m_steamIDLobby); // I'm not sure if this is needed
	this->m_fields->m_enterLobbyCallResult.Set(this->m_fields->m_lobby, this, &MyGameManager::onLobbyEnter);
}

void MyGameManager::fetchMemberList() {
	int lobbyMemberCount = SteamMatchmaking()->GetNumLobbyMembers(m_fields->m_lobbyId);
	SteamNetworkingIdentity member;

	for (int i = 0; i < lobbyMemberCount; i++) {
		CSteamID steamIDFriend = SteamMatchmaking()->GetLobbyMemberByIndex(m_fields->m_lobbyId, i);
		member.SetSteamID(steamIDFriend);
		m_fields->m_playersInLobby.push_back(member);   
	} 
}

// TODO: EResult? Vectors?
// Sends data to all members in current lobby
void MyGameManager::sendDataToMembers(const void* data) {
	for (auto const& member : this->m_fields->m_playersInLobby) {
        log::debug("SendData called on some user!");
		SteamNetworkingMessages()->SendMessageToUser(member, data, sizeof(data), k_nSteamNetworkingSend_Reliable, 0);
	}
}

void MyGameManager::receiveData() {
	SteamNetworkingMessage_t* messageList[32];
	auto numMessages = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, messageList, 32);
	for (int i = 0; i < numMessages; i++) {
		log::debug("Data received!");
		SteamNetworkingMessage_t* msg = messageList[i];
		auto res = matjson::parse(static_cast<const char*>(msg->GetData()));

		// This could be removed later on
		if (!res) {
			log::error("Failed to parse json: {}", res.unwrapErr());
			return;
		}

		// TODO: Check type
		matjson::Value unwrappedMessage = res.unwrap();


		// TODO: Validate position and ID
		auto gameObj = GameObject::createWithKey((int) unwrappedMessage["ObjID"].asInt().unwrap());
		gameObj->setPosition({(float) unwrappedMessage["x"].asInt().unwrap(), (float) unwrappedMessage["y"].asInt().unwrap()} );

		this->m_fields->m_level->addSpecial(gameObj);
	}
}

void MyGameManager::update(float p0) {
	SteamAPI_RunCallbacks();

	if (this->m_fields->m_isInLobby) {
		this->receiveData();
	}
	GameManager::update(p0);
}