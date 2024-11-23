#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <isteammatchmaking.h>
#include <isteamnetworkingmessages.h>
#include <isteamuser.h>
#include "../ActionTypes.hpp"
#include "ModifyGameManager.hpp"


using namespace geode::prelude;

void CallbackManager::onGameJoinRequestWrapper(GameLobbyJoinRequested_t* pCallback) {
	auto gameManager = static_cast<MyGameManager*>(GameManager::get());
	gameManager->onGameJoinRequest(pCallback);
}

void CallbackManager::onLobbyDataUpdateWrapper(LobbyDataUpdate_t* pCallback) {
	auto gameManager = static_cast<MyGameManager*>(GameManager::get());
	gameManager->fetchMemberList();
}

void MyGameManager::onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure) {
	if (pCallback->m_eResult == k_EResultOK) {
		log::info("Created Lobby!");

		// Although this would work, this shouldnt be relied on for checking if
		// the player is in the editor layer.
		// TODO: Change this inside EditorLayer::init instead!
		m_fields->m_isInEditorLayer = true;

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
	m_fields->m_isInLobby = true;
	log::info("EnterLobby called!");
	if (m_fields->m_isHost) {
		log::info("Entered Lobby as host, not doing anything anymore!");
		return;
	}
	else {
		m_fields->m_isInEditorLayer = true;
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

	log::debug("Fetch Memberlist Called; Fetching {} users:", lobbyMemberCount);

	for (int i = 0; i < lobbyMemberCount; i++) {
		CSteamID steamIDFriend = SteamMatchmaking()->GetLobbyMemberByIndex(m_fields->m_lobbyId, i);
		if (steamIDFriend == SteamUser()->GetSteamID()) {
			continue;
		}

		member.SetSteamID(steamIDFriend);
		log::debug("{} with id: {}", SteamFriends()->GetFriendPersonaName(steamIDFriend), steamIDFriend.ConvertToUint64());
		m_fields->m_playersInLobby.push_back(member);   
	} 
}

// TODO: EResult? Vectors?
// Sends data to all members in current lobby
void MyGameManager::sendDataToMembers(const char* data) {
	for (auto const& member : this->m_fields->m_playersInLobby) {
        log::debug("SendData called on some user!");
		SteamNetworkingMessages()->SendMessageToUser(member, data, static_cast<uint32>(strlen(data)), k_nSteamNetworkingSend_Reliable, 0);
	}
}

void MyGameManager::receiveData() {
	SteamNetworkingMessage_t* messageList[32];
	auto numMessages = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, messageList, 32);
	for (int i = 0; i < numMessages; i++) {
		SteamNetworkingMessage_t* msg = messageList[i];
		auto res = matjson::parse(static_cast<const char*>(msg->GetData()));

		log::debug("Data received! {} ", static_cast<const char*>(msg->GetData()));

		if (!res) {
			log::error("Failed to parse json: {}", res.unwrapErr());
			return;
		}

		matjson::Value unwrappedMessage = res.unwrap();

		auto level = LevelEditorLayer::get();

		if (!level) {
			level = this->m_fields->m_level;
		}

		switch ((int) unwrappedMessage["Type"].asInt().unwrapOr(-1)) {
			case eBlockPlaced: {
				// TODO: Validate position and ID
				auto gameObj = GameObject::createWithKey((int) unwrappedMessage["ObjID"].asInt().unwrap());
				gameObj->setPosition({(float) unwrappedMessage["x"].asInt().unwrap(), (float) unwrappedMessage["y"].asInt().unwrap()} );
				level->addSpecial(gameObj);
				break;
			}

			default:
				log::warn("No case switch found! Are you sure you are on the right version?");
		}
		msg->Release();
	}
}

void MyGameManager::update(float p0) {
	SteamAPI_RunCallbacks();

	if (this->m_fields->m_isInLobby) {
		this->receiveData();
	}

	GameManager::update(p0);
}