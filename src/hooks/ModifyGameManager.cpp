#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <isteammatchmaking.h>
#include <isteamnetworkingmessages.h>
#include "ModifyGameManager.hpp"


using namespace geode::prelude;


// bool MyGameManager::init() {

// 	if (!GameManager::init()) {
// 		return false;
// 	}

// 	return true;
// }

void MyGameManager::onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure) {
	if (pCallback->m_eResult == k_EResultOK) {
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
	if (m_fields->m_isHost) {
		log::info("Entered Lobby as host, not doing anything anymore!");
		return;
	}
	else {
    	CCDirector::get()->replaceScene(CCTransitionFade::create(0.5, this->m_fields->m_level.scene(GJGameLevel::create(), false)));
	}
}

// I am slowly devolving into madness
void MyGameManager::onGameJoinRequest(GameLobbyJoinRequested_t* pCallback) {

	geode::createQuickPopup(
		"Lobby?",            // title
		"Join Lobby?",   // content
		"Nah", "Yeah",      // buttons
		[&](auto, bool btn2) {
			if (btn2) {
				log::debug("Joining game with steamID: {}", pCallback->m_steamIDFriend.ConvertToUint64());
				auto lobby = SteamMatchmaking()->JoinLobby(pCallback->m_steamIDLobby); // I'm not sure if this is needed
			}
		}
	);
}

void MyGameManager::onGameOverlayActivated(GameOverlayActivated_t* pCallback) {
	log::info("Callbacks work!");
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
		SteamNetworkingMessages()->SendMessageToUser(member, data, sizeof(data), k_nSteamNetworkingSend_Reliable, 0);
	}
}

void MyGameManager::receiveData() {
	SteamNetworkingMessage_t* messageList[32];
	auto numMessages = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, messageList, 32);
	for (int i = 0; i < numMessages; i++) {
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

		this->m_fields->m_level.addSpecial(gameObj);
	}
}


void MyGameManager::update(float p0) {
	SteamAPI_RunCallbacks();

	if (this->m_fields->m_isInLobby) {
		this->receiveData();
	}
	GameManager::update(p0);
}