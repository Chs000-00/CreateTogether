#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <isteammatchmaking.h>
#include <isteamnetworkingmessages.h>
#include <isteamuser.h>
#include "../ActionTypes.hpp"
#include "../LobbyData.hpp"
#include "../layers/LobbiesLayer.hpp"
#include "ModifyEditorLayer.hpp"
#include "ModifyGameObject.hpp"
#include "ModifyGameManager.hpp"


using namespace geode::prelude;

#define GET_OBJECT_FROM_UID (GameObject*)level->m_fields->m_pUniqueIDOfGameObject->objectForKey(unwrappedMessage["ObjectUID"].asInt().ok().value())
#define GET_CCPOINT static_cast<float>(unwrappedMessage["x"].asInt().ok().value()), static_cast<float>(unwrappedMessage["y"].asInt().ok().value())
#define VALIDATE_MESSAGE(key, type) if (unwrappedMessage[key].as ## type().isErr()) continue

void CallbackManager::onGameJoinRequest(GameLobbyJoinRequested_t* pCallback) {

	auto callback = new GameLobbyJoinRequested_t;
	callback->m_steamIDFriend = pCallback->m_steamIDFriend;
	callback->m_steamIDLobby = pCallback->m_steamIDLobby;

	geode::createQuickPopup(
		"Lobby",         
		"Join Lobby?",
		"Nah", "Yeah",
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


	if (gameManager->m_fields->m_lobbyId != pCallback->m_ulSteamIDLobby) {
		// log::debug("Mismatching Update! {} != {}", gameManager->m_fields->m_lobbyId, pCallback->m_ulSteamIDLobby);
		return;
	}

	log::debug("LobbyChatUpdateWrapper called. UserID: {} | UserName: {} | StateChange: {} | SteamIDMakingChange: {}", pCallback->m_ulSteamIDUserChanged, SteamFriends()->GetFriendPersonaName(pCallback->m_ulSteamIDUserChanged), pCallback->m_rgfChatMemberStateChange, pCallback->m_ulSteamIDMakingChange);


	gameManager->fetchMemberList();
}

void CallbackManager::onNetworkingMessagesSessionRequest(SteamNetworkingMessagesSessionRequest_t* pCallback) {
	SteamNetworkingMessages()->AcceptSessionWithUser(pCallback->m_identityRemote);
}

void CallbackManager::onLobbyEnter(LobbyEnter_t* pCallback) {

	if (pCallback->m_EChatRoomEnterResponse != k_EChatRoomEnterResponseSuccess) {
		log::error("Failed to enter lobby with error code {}", pCallback->m_EChatRoomEnterResponse);
		FLAlertLayer::create(
			"Lobby Error",  
			fmt::format("Failed to enter lobby; <cr>Error {} </c>", pCallback->m_EChatRoomEnterResponse),
			"Ok"   
		)->show();
		return;
	}


	auto gameManager = static_cast<MyGameManager*>(GameManager::get());
	log::info("EnterLobby called with! SteamID: {} | ChatRoomEnterResponse: {}", pCallback->m_ulSteamIDLobby, pCallback->m_EChatRoomEnterResponse);

	gameManager->m_fields->m_isInLobby = true;

	if (gameManager->m_fields->m_isHost) {
		log::warn("onLobbyEntered called as host!");
		gameManager->m_fields->m_level = LevelEditorLayer::get();
	} else {
		gameManager->m_fields->m_isInEditorLayer = true;
		gameManager->m_fields->m_lobbyId = pCallback->m_ulSteamIDLobby;
		gameManager->m_fields->m_level = LevelEditorLayer::create(GJGameLevel::create(), false);
		gameManager->fetchMemberList();
		switchToScene(gameManager->m_fields->m_level);
	}
}

void MyGameManager::onLobbyMatchList(LobbyMatchList_t *pLobbyMatchList, bool bIOFailure) {	
	auto lobbyCount = pLobbyMatchList->m_nLobbiesMatching;
	lobbyData clobby;

	for (int i = 0; i < lobbyCount; i++) {
		CSteamID lobbyID = SteamMatchmaking()->GetLobbyByIndex(i);

		if (SteamMatchmaking()->GetLobbyData(lobbyID, "level_name") != MOD_VERSION) {
			clobby.isVersionMismatched = true;
			continue;
		}

		SteamMatchmaking()->RequestLobbyData(lobbyID);

		clobby.levelName = SteamMatchmaking()->GetLobbyData(lobbyID, "level_name");
		clobby.steamUserName = SteamMatchmaking()->GetLobbyData(lobbyID, "host_name");
		clobby.steamId = lobbyID;

		this->m_fields->m_lobbyLayer->m_data->push_back(clobby);   
		// log::debug("Data stuff: {} | {}", clobby.steamId.ConvertToUint64(), steamIDFriend.ConvertToUint64());

	}
	this->m_fields->m_lobbyLayer->loadDataToList();
}


void MyGameManager::onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure) {
	if (pCallback->m_eResult == k_EResultOK) {
		log::info("Created Lobby with steamID {} !", pCallback->m_ulSteamIDLobby);


		FLAlertLayer::create(
			"Lobby host",
			"Lobby created successfully!",
			"Ok"
		)->show();

		// Although this would work, this shouldnt be relied on for checking if
		// the player is in the editor layer.
		// TODO: Change this inside EditorLayer::init instead!
		m_fields->m_isInEditorLayer = true;

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
		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "level_name", LevelEditorLayer::get()->m_level->m_levelName.c_str());
		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "host_name", SteamFriends()->GetPersonaName());

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
        // log::debug("SendData called on {}", SteamFriends()->GetFriendPersonaName(member.GetSteamID()));
		SteamNetworkingMessages()->SendMessageToUser(member, data, static_cast<uint32>(strlen(data)), k_nSteamNetworkingSend_Reliable, 0);
	}
	// log::debug("Done sending messages");
}

bool MyGameManager::validateData(matjson::Value data) {

	if (!data.contains("Type")) {
		log::warn("Message does not contain a Type!");
		return false;
	}

	return true;
}

void MyGameManager::receiveData() {
	SteamNetworkingMessage_t* messageList[32];
	auto numMessages = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, messageList, 32);
	for (int i = 0; i < numMessages; i++) {
		SteamNetworkingMessage_t* msg = messageList[i];
		auto res = matjson::parse(static_cast<std::string>(static_cast<const char*>(msg->GetData())).append("\0"));
		msg->Release();

		log::debug("Data received: {} ", static_cast<const char*>(msg->GetData()));

		if (!res) {
			log::error("Failed to parse json: {}", res.unwrapErr());
			continue;
		}

		matjson::Value unwrappedMessage = res.unwrap();

		if (!MyGameManager::validateData(unwrappedMessage)) {
			continue;
		}

		auto level = static_cast<MyLevelEditorLayer*>(this->m_fields->m_level);

		// TODO: Try using results instead
		// TODO: Check amd validate the data 

		auto type = (int)unwrappedMessage["Type"].asInt().unwrapOr(-1);
		switch (type) {
			case eActionPlacedObject: {
				VALIDATE_MESSAGE("ObjID", UInt);
				VALIDATE_MESSAGE("x", Int);
				VALIDATE_MESSAGE("y", Int);
				
				auto gameObjectID = unwrappedMessage["ObjID"].asUInt().ok().value();
				cocos2d::CCPoint gameObjectPos = {GET_CCPOINT};
				
				// TODO: Figure if a race condition is possible
				level->m_fields->m_wasDataSent = true;
				GameObject* placedGameObject = level->createObject(gameObjectID, gameObjectPos, false);
				level->m_fields->m_wasDataSent = false;
				level->m_fields->m_pUniqueIDOfGameObject->setObject(placedGameObject, placedGameObject->m_uniqueID);

				break;
			}
			
			// TODO: Check if this works
			case eActionUpdatedFont: {
				VALIDATE_MESSAGE("FontID", Int);
				
				level->m_fields->m_wasDataSent = true;
				level->updateLevelFont(unwrappedMessage["FontID"].asInt().unwrap());
				level->m_fields->m_wasDataSent = false;
				break;
			}

			case eActionDeletedObject: {
				VALIDATE_MESSAGE("ObjectUID", Int);

				auto deletedObject = GET_OBJECT_FROM_UID;
				auto betterDeletedObject = static_cast<MyGameObject*>(deletedObject);
				betterDeletedObject->m_fields->m_wasDataSent = true;
				level->deleteObject(deletedObject);
				betterDeletedObject->m_fields->m_wasDataSent = false;
				break;
			}

			case eActionMovedObject: {
				VALIDATE_MESSAGE("ObjectUID", UInt);
				VALIDATE_MESSAGE("x", Int);
				VALIDATE_MESSAGE("y", Int);

				auto movedObject = GET_OBJECT_FROM_UID;
				auto betterDeletedObject = static_cast<MyGameObject*>(movedObject);
				cocos2d::CCPoint newPos = {GET_CCPOINT};
				betterDeletedObject->m_fields->m_wasDataSent = true;
				level->setPosition(newPos);
				betterDeletedObject->m_fields->m_wasDataSent = false;
				break;
			}

			default:
				log::warn("Type {} not found! Are you sure you're on the right version?", type);
		}
	}
}

void MyGameManager::update(float p0) {
	SteamAPI_RunCallbacks();

	if (this->m_fields->m_isInLobby) {
		this->receiveData();
	}

	GameManager::update(p0);
}