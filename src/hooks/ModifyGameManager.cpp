
#include "../config.hpp"

#ifdef USE_TEST_SERVER
	#include <WinSock2.h>
	#include "../types/PlaceboMsg.hpp"
#endif

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <isteammatchmaking.h>
#include <isteamnetworkingmessages.h>
#include <isteamuser.h>
#include "../types/ActionTypes.hpp"
#include "../types/LobbyData.hpp"
#include "../layers/LobbiesLayer.hpp"
#include "../ui/WaitingForHostPopup.hpp"
#include "ModifyEditorUI.hpp"
#include "ModifyEditorLayer.hpp"
#include "ModifyGameObject.hpp"
#include "ModifyGameManager.hpp"
#include "../utils/Utills.hpp"


using namespace geode::prelude;
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

				

			}
			delete callback;
		}
		
	);
}

void CallbackManager::onLobbyChatUpdateWrapper(LobbyChatUpdate_t* pCallback) {
	auto gameManager = static_cast<MyGameManager*>(GameManager::get());


	if (gameManager->m_fields->m_lobbyId != pCallback->m_ulSteamIDLobby) {
		// log::debug("Mismatching Update! {} != {}", gameManager->m_fields->m_lobbyId, pCallback->m_ulSteamIDLobby);
		return;
	}

	if (pCallback->m_ulSteamIDUserChanged == gameManager->m_fields->m_hostID.ConvertToUint64()) {
		if (pCallback->m_rgfChatMemberStateChange == k_EChatMemberStateChangeLeft || pCallback->m_rgfChatMemberStateChange == k_EChatMemberStateChangeDisconnected) {
			log::info("Host left server! Leaving lobby.");

			LobbiesLayer::scene();
			gameManager->leaveLobby();

			FLAlertLayer::create(
				"Host left server",    
				"The host has left the server!",  
				"Ok"
			)->show();

		}
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
	} else {
		gameManager->m_fields->m_lobbyId = pCallback->m_ulSteamIDLobby;
		gameManager->enterLevelEditor();
	}

	gameManager->m_fields->m_hostID = SteamMatchmaking()->GetLobbyOwner(gameManager->m_fields->m_lobbyId);

	// Make sure we did become host of a level who's host already left.
	if (!gameManager->m_fields->m_isHost && gameManager->m_fields->m_hostID == SteamUser()->GetSteamID()) {
		log::warn("Invalid host! onLobbyEntered was not called as host yet hostID is your steamID");
		gameManager->leaveLobby();
	}

}

void MyGameManager::enterLevelEditor() {
	WaitingForHostPopup::create();
	this->m_fields->m_isRequestingLevelString = true;
	this->m_fields->m_isInEditorLayer = true;
	auto gameLevel = GJGameLevel::create();
	gameLevel->m_isEditable = true;
	gameLevel->m_levelType = GJLevelType::Editor;
	// gameLevel->m_levelDesc += "Created with Create Together";
	auto lev = LevelEditorLayer::create(gameLevel, false);
	// TODO: Only send this to host!!!!!!!!!
	auto msg = "{\"Type\": 7}";


    #ifndef USE_TEST_SERVER
        SteamNetworkingIdentity host;
        host.SetSteamID(this->m_fields->m_hostID);
        SteamNetworkingMessages()->SendMessageToUser(host, msg, static_cast<uint32>(strlen(msg)), k_nSteamNetworkingSend_Reliable, 0);
    #else
		// FIX! THIS!
        sendDataToMembers(msg, false);
    #endif

	this->fetchMemberList();
	switchToScene(lev);
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
		// m_fields->m_level = LevelEditorLayer::get();

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
		CSteamID steamIDLobbyMember = SteamMatchmaking()->GetLobbyMemberByIndex(m_fields->m_lobbyId, i);
		if (steamIDLobbyMember == SteamUser()->GetSteamID()) {
			log::debug("(Self) - {} with SteamID: {}", SteamFriends()->GetPersonaName(), steamIDLobbyMember.ConvertToUint64());
			continue;
		}

		member.SetSteamID(steamIDLobbyMember);
		log::debug("{} with SteamID: {}", SteamFriends()->GetFriendPersonaName(steamIDLobbyMember), steamIDLobbyMember.ConvertToUint64());
		m_fields->m_playersInLobby.push_back(member);   
	} 
}

// TODO: EResult? Vectors?
// Sends data to all members in current lobby
void MyGameManager::sendDataToMembers(std::string data, bool receiveData) {

	// Make sure we didn't send before receiving data
	// This most likely won't be needed
	// TODO: Check if I can remove this
	if (receiveData) {
		this->receiveData();
	}

	log::info("Sending MSG {} {}", data, static_cast<uint32>(strlen(data.c_str())));

	#ifndef USE_TEST_SERVER

        auto fixedData = data.c_str();
        

		for (auto const& member : this->m_fields->m_playersInLobby) {
			// log::debug("SendData called on {}", SteamFriends()->GetFriendPersonaName(member.GetSteamID()));
			SteamNetworkingMessages()->SendMessageToUser(member, fixedData, static_cast<uint32>(strlen(fixedData)), k_nSteamNetworkingSend_Reliable, 0);
		}
		// log::debug("Done sending messages");

	#else
		send(this->m_fields->m_socket, data.c_str(), strlen(data.c_str()), 0);
	#endif
}


bool MyGameManager::validateData(matjson::Value data) {

	if (!data.contains("Type")) {
		log::warn("Message does not contain a Type!");
		return false;
	}

	return true;
}

Result<int> MyGameManager::parseDataReceived(matjson::Value data, NETWORKING_MSG* msg) {

		// I can get rid of this to save on 10 nanoseconds!
		if (!LevelEditorLayer::get()) {
			return Err("WTF? parseDataReceived called yet LevelEditorLayer::get returned nullptr");
		}

		auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());

		auto type = data["Type"].asInt().unwrapOr(-1);
		switch (type) {
			case eActionPlacedObject: {
				log::debug("obj placed called");
				GEODE_UNWRAP_INTO(int gameObjectID, data["ObjID"].asUInt());
				GEODE_UNWRAP_INTO(double x, data["x"].asDouble());
				GEODE_UNWRAP_INTO(double y, data["y"].asDouble());
				GEODE_UNWRAP_INTO(std::string uid, data["ObjectUID"].asString());

				cocos2d::CCPoint gameObjectPos = {static_cast<float>(x), static_cast<float>(y)};
				
				// TODO: Figure if a race condition is possible
				level->m_fields->m_wasDataSent = true;
				GameObject* placedGameObject = level->createObject(gameObjectID, gameObjectPos, false);
				level->m_fields->m_wasDataSent = false;

				MyGameObject* betterPlacedGameObject = static_cast<MyGameObject*>(placedGameObject);

				betterPlacedGameObject->m_fields->m_veryUniqueID = uid;

				if (level->m_fields->m_pUniqueIDOfGameObject->objectForKey(betterPlacedGameObject->m_fields->m_veryUniqueID)) {
					return Err("eActionPlacedObject: UID Already exists");
					break;
				}

				// Works the same with asBool as UseExtra is not part of the json when it is false
				// TODO: Rewrite but with GEODE_UNWRAP_INTO
				// GEODE_UNWRAP_IF_OK(UseExtra)
				// TODO: Find a better way to do this
				if (data.contains("Rot") && data["Rot"].asInt().isOk())
					betterPlacedGameObject->setRotation(data["Rot"].asInt().ok().value());

				if (data.contains("HD") && data["HD"].asBool().isOk())
					betterPlacedGameObject->m_isHighDetail = data["HD"].asBool().ok().value();

				if (data.contains("NoGlow") && data["NoGlow"].asBool().isOk())
					betterPlacedGameObject->m_hasNoGlow = data["NoGlow"].asBool().ok().value();

				if (data.contains("NoEnter") && data["NoEnter"].asBool().isOk())
					betterPlacedGameObject->m_isDontEnter = data["NoEnter"].asBool().ok().value();
					
				if (data.contains("FlipX") && data["FlipX"].asBool().isOk())
					betterPlacedGameObject->setFlipX(data["FlipX"].asBool().ok().value());

				if (data.contains("FlipY") && data["FlipY"].asBool().isOk())
					betterPlacedGameObject->setFlipY(data["FlipY"].asBool().ok().value());				

				level->m_fields->m_pUniqueIDOfGameObject->setObject(placedGameObject, betterPlacedGameObject->m_fields->m_veryUniqueID);

				break;
			}
			
			// FIXME: This uses the old deleting logic, which is incompatible with the new one!
			case eActionDeletedObject: {

				GEODE_UNWRAP_INTO(std::vector deletedObjects, data["EditUUIDs"].asArray());

				for (auto obj : deletedObjects) {
					if (obj.isString()) {
						auto dObj = (GameObject*)level->m_fields->m_pUniqueIDOfGameObject->objectForKey(obj.asString().ok().value());
						level->deleteObject(dObj);
					}
				}


				break;
			}

			case eActionTransformObject: {
				GEODE_UNWRAP_INTO(std::string uid, data["ObjectUID"].asString());
				GEODE_UNWRAP_INTO(int command, data["EditCommand"].asInt());

				auto transformedObject = static_cast<GameObject*>(level->m_fields->m_pUniqueIDOfGameObject->objectForKey(uid));

				if (!transformedObject) {
					return Err("eActionTransformObject: Object UID not found");
					break;
				}

				auto cEditorUI = static_cast<MyEditorUI*>(level->m_editorUI);
				// TODO: Check command range

				cEditorUI->m_fields->m_wasDataSent = true;
				cEditorUI->transformObject(transformedObject, static_cast<EditCommand>(command), false);
				cEditorUI->m_fields->m_wasDataSent = false;
				break;
			}

			case eActionMovedObject: {

				if (!data.contains("EditUUIDs") && !data["EditUUIDs"].isObject()) {
					return Err("eActionMovedObject: No EditUUIDs object");
				}

				auto movedObjects = data["EditUUIDs"];

                // Questionabel 
				for (auto obj = movedObjects.begin(); obj != movedObjects.end(); ++obj) {

                    // Very Questionabel code, TODO: REWRITE THIS!
					// TODO: Check if X and/or Y exists!
					if (obj->isObject()) {
						auto dObj = static_cast<GameObject*>(level->m_fields->m_pUniqueIDOfGameObject->objectForKey(obj->getKey().value()));

                        if (!dObj) {
							return Err("eActionMovedObject: dObj does not exist!");
                            break;
                        }

						 auto pos = movedObjects[obj->getKey().value()];

						if (pos["x"].asInt().isErr() || pos["y"].asInt().isErr()) {
							return Err("eActionMovedObject: x and/or y missing!");
						}

                        CCPoint newPos = {static_cast<float>(pos["x"].asInt().ok().value()), static_cast<float>(pos["y"].asInt().ok().value())};
						dObj->setPosition(newPos);
					}
				}

				//auto cEditorUI = static_cast<MyEditorUI*>(level->m_editorUI);
				//cEditorUI->m_fields->m_wasDataSent = true;

				// Although this code might work better, it causes to desync issues
				// cEditorUI->moveObject(transformedObject, newPos);
				//cEditorUI->m_fields->m_wasDataSent = false;
				break;
			}

			case eActionRequestLevel: {

                if (!this->m_fields->m_isHost) { 
					return Err("eActionRequestLevel: User request level string from Non-Host");
                }

				matjson::Value lvlStringJson = this->getLevelStringMatjson();
				auto out = lvlStringJson.dump(matjson::NO_INDENTATION).c_str();

				sendDataToUser(msg->m_identityPeer, out);

				break;
			}

			case eActionReturnLevelString: {

				if (!this->m_fields->m_isRequestingLevelString) {
					return Err("eActionReturnLevelString: Not requesting a level string");
				}

				#ifndef USE_TEST_SERVER
					if (this->m_fields->m_hostID != msg->m_identityPeer.GetSteamID()) {
						return Err("eActionReturnLevelString: Non-Host level string");
						break;
					}
				#endif

				GEODE_UNWRAP_INTO(std::vector editUUIDs, data["EditUUIDs"].asArray());
				GEODE_UNWRAP_INTO(std::string levelString, data["LevelString"].asString());

                // IDfk what the args do
				// TODO: Figure out args
				auto objectArr = CCArrayExt<MyGameObject*>(level->createObjectsFromString(levelString, false, false));

				

				// This might be inefficient as this requires looping over the arr twice.
				for (auto i = 0; i != std::min(objectArr.size(), editUUIDs.size()); ++i) {
					auto mObject = (objectArr[i]);
					GEODE_UNWRAP_INTO(std::string uid, editUUIDs[i].asString());
					mObject->m_fields->m_veryUniqueID = uid;
					level->m_fields->m_pUniqueIDOfGameObject->setObject(mObject, uid);
				}

				this->m_fields->m_isRequestingLevelString = false;

				break;
			}

			case eActionPastedObjects: {
				GEODE_UNWRAP_INTO(std::vector editUUIDs, data["EditUUIDs"].asArray());
				GEODE_UNWRAP_INTO(std::string objectString, data["ObjectString"].asString());



				// TODO: Figure out args
				auto objectArr = CCArrayExt<MyGameObject*>(level->createObjectsFromString(objectString, false, false));


				// This might be inefficient as this requires looping over the arr twice.
				for (auto i = 0; i != std::min(objectArr.size(), editUUIDs.size()); ++i) {
					auto mObject = (objectArr[i]);
					GEODE_UNWRAP_INTO(std::string uid, editUUIDs[i].asString());
					mObject->m_fields->m_veryUniqueID = uid;
					level->m_fields->m_pUniqueIDOfGameObject->setObject(mObject, uid);
				}
			}

			case eActionModifiedObject: {
				GEODE_UNWRAP_INTO(std::vector editUUIDs, data["EditUUIDs"].asArray());
				GEODE_UNWRAP_INTO(std::string objectString, data["ObjectString"].asString());


				// TODO: Figure out args
				auto objectArr = CCArrayExt<MyGameObject*>(level->createObjectsFromString(objectString, false, false));

				if (objectArr.size() != editUUIDs.size()) {
					return Err("eActionModifiedObject: Mismatched objectArr and editUUIDs size!");
				}

				// This might be inefficient as this requires looping over the arr twice.
				for (auto i = 0; i != std::min(objectArr.size(), editUUIDs.size()); ++i) {
					auto newObj = (objectArr[i]);

					GEODE_UNWRAP_INTO(std::string uid, editUUIDs[i].asString());

					if (auto oldObj = static_cast<MyGameObject*>(level->m_fields->m_pUniqueIDOfGameObject->objectForKey(uid))) {
						level->deleteObject(oldObj);
						newObj->m_fields->m_veryUniqueID = uid;
						level->m_fields->m_pUniqueIDOfGameObject->setObject(newObj, uid);
					}
					else {
						return Err("eActionTransformObject: Object UID not found");
					}
				}
				break;
			}


			// TODO: Check out SelectFontLayer
			case eActionUpdatedFont: {
				GEODE_UNWRAP_INTO(int fontID, data["FontID"].asInt());



				level->m_fields->m_wasDataSent = true;
				level->updateLevelFont(fontID);
				level->m_fields->m_wasDataSent = false;

				level->levelSettingsUpdated();

				// if (auto settingsLayer = CCScene::get()->getChildByType<LevelSettingsLayer>(0)) {
				// 	settingsLayer->selectSettingClosed();
				// }

				break;
			}

			case eActionChangeGroupID: {
				GEODE_UNWRAP_INTO(int groupID, data["GroupID"].asInt());
				GEODE_UNWRAP_INTO(bool isAdding, data["Add"].asBool());
				GEODE_UNWRAP_INTO(std::vector editUUIDs, data["EditUUIDs"].asArray());


				// This might be inefficient as this requires looping over the arr twice.
				for (auto i = 0; i != editUUIDs.size(); ++i) {
					GEODE_UNWRAP_INTO(std::string uid, editUUIDs[i].asString());

					level->m_fields->m_wasDataSent = true;

					auto retObj = static_cast<GameObject*>(level->m_fields->m_pUniqueIDOfGameObject->objectForKey(uid));

					if (isAdding) {
						level->addToGroup(retObj, groupID, false);
						retObj->addToGroup(groupID);
					} else {
						level->removeFromGroup(retObj, groupID);
						retObj->removeFromGroup(groupID);
					}

					level->m_fields->m_wasDataSent = false;
				}	
				break;
			}

            case eActionSongChanged: {
				GEODE_UNWRAP_INTO(int songID, data["SongID"].asUInt());
                level->m_level->m_songID = songID;
				level->levelSettingsUpdated();
				break;
            }

            case eActionArtSelected: {
				GEODE_UNWRAP_INTO(int artType, data["ArtType"].asInt());
				GEODE_UNWRAP_INTO(int artInt, data["Art"].asInt());                

                // TODO: Check SelectArtType range

				if (!isValidEnumRange(artType, 0, constants::LARGEST_SELECT_ART_TYPE)) {
					return Err("eActionArtSelected: Invalid range");
				}

				auto artTypeCasted = static_cast<SelectArtType>(artType);

                if (artTypeCasted == SelectArtType::Ground) {
					GEODE_UNWRAP_INTO(int line, data["Line"].asInt());             
                    level->createGroundLayer(artInt, line);
                }


                if (artTypeCasted == SelectArtType::Background) {
                    level->createBackground(artInt);
                }

				break;

            }

			case eOptionSpeedChanged: {
				GEODE_UNWRAP_INTO(int speed, data["Speed"].asInt());     

				if (!isValidEnumRange(speed, 0, constants::LARGEST_SPEED)) {
					return Err("eOptionSpeedChanged: Invalid range");
					break; // Just in case
				}

				level->m_levelSettings->m_startSpeed = static_cast<Speed>(speed);

				level->levelSettingsUpdated();

				break;
			}

			case eOptionGameModeChanged: {
				GEODE_UNWRAP_INTO(int gameMode, data["GameMode"].asInt());    
				level->m_levelSettings->m_startMode = gameMode;
				level->levelSettingsUpdated();
				break;
			}

			// TODO: Debug thsi why is it not working wtf
			case eOptionLevelSetting: {
				GEODE_UNWRAP_INTO(int toggleIndex, data["ToggleIndex"].asInt());   

				if (toggleIndex == 13) {
					GEODE_UNWRAP_INTO(float val, data["Val"].asDouble());   
					level->m_levelSettings->m_spawnGroup = val;
				} 
				else {
					auto ret = toggleFromLevelSettings(level->m_levelSettings, toggleIndex);

					if (!ret) {
						return ret;
					}
				}
				break;
			}

			case eActionRotatedObject: {
				GEODE_UNWRAP_INTO(float rotation, data["Rot"].asDouble());   
				GEODE_UNWRAP_INTO(float x, data["x"].asDouble());
				GEODE_UNWRAP_INTO(float y, data["y"].asDouble());
				GEODE_UNWRAP_INTO(std::vector editUUIDs, data["EditUUIDs"].asArray());

				cocos2d::CCPoint position = {x, y};

				auto cEditorUI = static_cast<MyEditorUI*>(level->m_editorUI);
				// TODO: Check command range


				auto arr = CCArray::create();

				for (auto i = 0; i < editUUIDs.size(); ++i) {
					GEODE_UNWRAP_INTO(std::string uid, editUUIDs[i].asString());
					auto retObj = static_cast<GameObject*>(level->m_fields->m_pUniqueIDOfGameObject->objectForKey(uid));
					arr->addObject(retObj);
				}
				
				cEditorUI->m_fields->m_wasDataSent = true;
				cEditorUI->rotateObjects(arr, rotation, position);
				cEditorUI->m_fields->m_wasDataSent = false;

				break;
			}

			default:
				log::warn("Type {} not found! Are you sure you're on the right version?", type);
				return Err("Invalid case switch");
		}



	return Ok(0);
}

void MyGameManager::receiveData() {

	if (this->m_fields->m_isInEditorLayer) {
		return;
	}

	#ifndef USE_TEST_SERVER
		SteamNetworkingMessage_t* messageList[128];
		auto numMessages = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, messageList, 128);
	#else
		// Compat with for loop
		auto numMessages = 1;

		// What is this used for?
		// char tserverdat[8192];
		// memset(tserverdat, 0, sizeof(tserverdat)); 
		TestServerMsg* msg = new TestServerMsg;
		auto outrec = recv(this->m_fields->m_socket, msg->m_data, sizeof(msg->m_data), 0);

		if (outrec == -1) {
			msg->Release();
			return;
		}
	#endif

	for (int i = 0; i < numMessages; i++) {
		
		#ifndef USE_TEST_SERVER
			SteamNetworkingMessage_t* msg = messageList[i];
		#endif

		// Fix weird issue with incorrect str termination
		auto fixedData = static_cast<std::string>(static_cast<const char*>(msg->GetData()));
		fixedData.resize(msg->GetSize());

		auto res = new geode::Result<matjson::Value, matjson::ParseError>(matjson::parse(fixedData));

		log::info("Data received: {} {} ", fixedData, msg->GetSize());
		

		if (res->isErr()) {
			log::error("Failed to parse json: {}", res->unwrapErr());
			delete res;
			res = nullptr;	// just in case
			msg->Release();
			continue;
		}

		matjson::Value unwrappedMessage = res->unwrap();

		if (!MyGameManager::validateData(unwrappedMessage)) {
			delete res;
			res = nullptr;	// just in case
			msg->Release();
			continue;
		}

		auto out = this->parseDataReceived(unwrappedMessage, msg);

		if (!out) {
			log::warn("Something went wrong with the parsing: {}", out.unwrapErr());
		}

		delete res;
		res = nullptr;	// just in case
		msg->Release();
	}
}

// TODO: Finish this
matjson::Value MyGameManager::getLevelStringMatjson() {

	// I can get rid of this to save on 10 nanoseconds!
	if (auto worseLevel = LevelEditorLayer::get()) {
		auto level = static_cast<MyLevelEditorLayer*>(worseLevel);

		matjson::Value rjson = matjson::makeObject({
			{"Type", static_cast<int>(eActionReturnLevelString)},
			{"LevelString", level->getLevelString()},
		});

		matjson::Value eUUIDs = matjson::Value::array();

		auto objectArr = CCArrayExt<MyGameObject*>(level->m_objects);

		// This might be inefficient as this requires looping over the arr twice.
		// Lazyness
		unsigned int index = 0;
		for (auto obj : objectArr) {
			eUUIDs.push(obj->m_fields->m_veryUniqueID);
			index += 1;
		}

		rjson["EditUUIDs"] = eUUIDs;

		return rjson;	
	}
	else {
		log::warn("WTF? getLevelStringMatjson called yet LevelEditorLayer::get() returned nullptr");
	}


}

void MyGameManager::sendDataToUser(SteamNetworkingIdentity usr, const char* out) {
	#ifndef USE_TEST_SERVER
		SteamNetworkingMessages()->SendMessageToUser(usr, out, static_cast<uint32>(strlen(out)), k_nSteamNetworkingSend_Reliable, 0);
	#else
        this->sendDataToMembers(out, false);
    #endif
}

void MyGameManager::lateSendData() {
	if (this->m_fields->m_sharedMassEdit.m_sendMoveList) {

		matjson::Value object = matjson::makeObject({
			{"Type", static_cast<int>(eActionMovedObject)},
			{"EditUUIDs", this->m_fields->m_sharedMassEdit.m_moveList}
		});

		this->sendDataToMembers(object.dump(matjson::NO_INDENTATION), false);
	
		this->m_fields->m_sharedMassEdit.m_sendMoveList = false;
	}


	if (this->m_fields->m_sharedMassEdit.m_sendGroupIDEdits) {

		matjson::Value object = matjson::makeObject({
			{"Type", static_cast<int>(eActionChangeGroupID)},
			{"Add", this->m_fields->m_sharedMassEdit.m_isAddingGroupID},
			{"GroupID", this->m_fields->m_sharedMassEdit.m_groupIDToEdit},
			{"EditUUIDs", this->m_fields->m_sharedMassEdit.m_groupIDEdits}
		});

		this->sendDataToMembers(object.dump(matjson::NO_INDENTATION), false);
	
		this->m_fields->m_sharedMassEdit.m_sendGroupIDEdits = false;
	}
}

void MyGameManager::update(float p0) {
	SteamAPI_RunCallbacks();

	if (this->m_fields->m_isInLobby) {
		this->receiveData();
		this->lateSendData();
	}

	GameManager::update(p0);
}

void MyGameManager::leaveLobby() {
	this->m_fields->m_isInEditorLayer = false;
	if (this->m_fields->m_isInLobby) {

        m_fields->m_isInLobby = false;

		#ifndef USE_TEST_SERVER

			log::info("Leaving lobby with ID {}", this->m_fields->m_lobbyId);
			SteamMatchmaking()->LeaveLobby(this->m_fields->m_lobbyId);
			this->m_fields->m_lobbyId = 0;

		#else
			// WHY DOES THIS CRASH????
			close(this->m_fields->m_socket);
		#endif

		// this->m_fields->m_lobbyCreated = 0;
		// this->m_fields->m_lobbyJoined = 0;

	}
	else {
		log::info("Can't leave lobby because not in lobby!");
	}
}