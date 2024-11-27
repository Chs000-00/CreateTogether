#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <isteammatchmaking.h>
#include <isteamnetworkingmessages.h>
#include <isteamuser.h>
#include "../ActionTypes.hpp"
#include "ModifyEditorLayer.hpp"
#include "ModifyGameManager.hpp"


using namespace geode::prelude;

void CallbackManager::onGameJoinRequestWrapper(GameLobbyJoinRequested_t* pCallback) {

	auto callback = new GameLobbyJoinRequested_t;
	callback->m_steamIDFriend = pCallback->m_steamIDFriend;
	callback->m_steamIDLobby = pCallback->m_steamIDLobby;

	// TODO: Remove this
	geode::createQuickPopup(
		"Lobby?",            // title
		"Join Lobby?",   // content
		"Nah", "Yeah",      // buttons
		[callback](auto, bool btn2) {
			if (btn2) {
				// TODO: Remove this
				auto gameManager = static_cast<MyGameManager*>(GameManager::get());
				log::debug("JoinLobbyRequest Called with steamID: {} | friendID: {} | friendName: {}", callback->m_steamIDLobby.ConvertToUint64(), callback->m_steamIDFriend.ConvertToUint64(), SteamFriends()->GetFriendPersonaName(callback->m_steamIDFriend));

				gameManager->m_fields->m_lobbyJoined = SteamMatchmaking()->JoinLobby(callback->m_steamIDLobby); // I'm not sure if this is needed

				delete callback;

			}
			else {
				delete callback;
			}
		}
		
	);
}

void CallbackManager::onLobbyChatUpdateWrapper(LobbyChatUpdate_t* pCallback) {
	auto gameManager = static_cast<MyGameManager*>(GameManager::get());

	log::debug("LobbyChatUpdateWrapper called! UserID: {} | UserName: {} | StateChange: {} | SteamIDMakingChange: {}", pCallback->m_ulSteamIDUserChanged, SteamFriends()->GetFriendPersonaName(pCallback->m_ulSteamIDUserChanged), pCallback->m_rgfChatMemberStateChange, pCallback->m_ulSteamIDMakingChange);

	if (gameManager->m_fields->m_lobbyId != pCallback->m_ulSteamIDLobby) {
		log::debug("Mismatching Update! {} != {}", gameManager->m_fields->m_lobbyId, pCallback->m_ulSteamIDLobby);
		return;
	}

	gameManager->fetchMemberList();
}

void CallbackManager::onLobbyEnter(LobbyEnter_t* pCallback) {

	if (pCallback->m_EChatRoomEnterResponse != k_EChatRoomEnterResponseSuccess) {
		log::error("Failed to enter lobby with error code {}", pCallback->m_EChatRoomEnterResponse);
		FLAlertLayer::create(
			"Lobby Error",  
			fmt::format("Failed to enter lobby; <cr>Error {} </c>", pCallback->m_EChatRoomEnterResponse),
			"OK"   
		)->show();
		return;
	}


	auto gameManager = static_cast<MyGameManager*>(GameManager::get());
	gameManager->fetchMemberList();
	log::info("EnterLobby called with! SteamID: {} | ChatRoomEnterResponse: {}", pCallback->m_ulSteamIDLobby, pCallback->m_EChatRoomEnterResponse);

	if (gameManager->m_fields->m_isHost) {
		log::warn("onLobbyEntered called as host!");
		gameManager->m_fields->m_level = LevelEditorLayer::get();
	} else {
		gameManager->m_fields->m_isInEditorLayer = true;
		gameManager->m_fields->m_lobbyId = pCallback->m_ulSteamIDLobby;
		gameManager->m_fields->m_isInLobby = true;
		gameManager->m_fields->m_level = LevelEditorLayer::create(GJGameLevel::create(), false);
		switchToScene(gameManager->m_fields->m_level);
	}
}

void MyGameManager::onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure) {
	if (pCallback->m_eResult == k_EResultOK) {
		log::info("Created Lobby with steamID {} !", pCallback->m_ulSteamIDLobby);

		// Although this would work, this shouldnt be relied on for checking if
		// the player is in the editor layer.
		// TODO: Change this inside EditorLayer::init instead!
		m_fields->m_isInEditorLayer = true;

		m_fields->m_isInLobby = true;
		m_fields->m_lobbyId = pCallback->m_ulSteamIDLobby;

		// Get pointer to the current level editor
		// Because this pointer would be destroyed, we would need to create a new LevelEditorLayer with the same data
		// instead of a pointer, so the LevelEditor would be presistent when leaving the level editor to playtest
		// Currently this is just a temporary solution to get the mod running
		m_fields->m_level = LevelEditorLayer::get();

		// Constants can be changed in CMakeLists.txt
		// Kind of a bad idea but who cares

		// SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "lobby_type", MOD_LOBBY_NAME); // TODO: Uncomment this
		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "version", MOD_VERSION);
		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "host_steamid", std::to_string(pCallback->m_ulSteamIDLobby).c_str());
		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "level_name", LevelEditorLayer::get()->m_level->m_levelName.c_str());
	}
	else {
		log::warn("Failed to create lobby with error code {}!", fmt::underlying(pCallback->m_eResult));
		
		m_fields->m_isInEditorLayer = false;
		m_fields->m_lobbyCreated = 0;
		m_fields->m_lobbyJoined = 0;
		m_fields->m_isInLobby = false;
	}
}

void MyGameManager::fetchMemberList() {
	int lobbyMemberCount = SteamMatchmaking()->GetNumLobbyMembers(m_fields->m_lobbyId);
	SteamNetworkingIdentity member;

	log::debug("FetchMemberList Called; Fetching {} users:", lobbyMemberCount);

	for (int i = 0; i < lobbyMemberCount; i++) {
		CSteamID steamIDFriend = SteamMatchmaking()->GetLobbyMemberByIndex(m_fields->m_lobbyId, i);
		if (steamIDFriend == SteamUser()->GetSteamID()) {
			log::debug("(Self) - {} with SteamID: {}", SteamFriends()->GetPersonaName(), steamIDFriend.ConvertToUint64());
			continue;
		}

		member.SetSteamID(steamIDFriend);
		log::debug("{} with SteamID: {}", SteamFriends()->GetFriendPersonaName(steamIDFriend), steamIDFriend.ConvertToUint64());
		m_fields->m_playersInLobby.push_back(member);   
	} 
}

// TODO: EResult? Vectors?
// Sends data to all members in current lobby
void MyGameManager::sendDataToMembers(const char* data) {
	for (auto const& member : this->m_fields->m_playersInLobby) {
        log::debug("SendData called on {}", SteamFriends()->GetFriendPersonaName(member.GetSteamID()));
		SteamNetworkingMessages()->SendMessageToUser(member, data, static_cast<uint32>(strlen(data)), k_nSteamNetworkingSend_Reliable, 0);
	}
	log::debug("Done sending messages");
}

void MyGameManager::receiveData() {
	SteamNetworkingMessage_t* messageList[32];
	auto numMessages = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, messageList, 32);
	for (int i = 0; i < numMessages; i++) {
		SteamNetworkingMessage_t* msg = messageList[i];
		auto res = matjson::parse(static_cast<std::string>(static_cast<const char*>(msg->GetData())).append("\0"));

		log::debug("Data received: {} ", static_cast<const char*>(msg->GetData()));

		if (!res) {
			log::error("Failed to parse json: {}", res.unwrapErr());
			return;
		}

		matjson::Value unwrappedMessage = res.unwrap();

		auto level = static_cast<MyLevelEditorLayer*>(this->m_fields->m_level);

		switch ((int) unwrappedMessage["Type"].asInt().unwrapOr(-1)) {
			case eActionPlacedObject: {
				// TODO: Validate position and ID
				auto gameObjectID = (int)unwrappedMessage["ObjID"].asInt().unwrap();
				cocos2d::CCPoint gameObjectPos = {(float)unwrappedMessage["x"].asInt().unwrap(), (float)unwrappedMessage["y"].asInt().unwrap()};
				
				// TODO: Figure out why it dosent
				level->m_fields->m_wasDataSent = true;
				level->createObject(gameObjectID, gameObjectPos, false);
				level->m_fields->m_wasDataSent = false;
				break;
			}

			case eActionUpdatedFont: {
				level->m_fields->m_wasDataSent = true;
				level->updateLevelFont(unwrappedMessage["FontID"].asInt().unwrap());
				level->m_fields->m_wasDataSent = false;
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