
#include "../config.hpp"

#ifdef USE_TEST_SERVER
	#include <WinSock2.h>
	#include "../types/PlaceboMsg.hpp"
#endif

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <ctserialize_generated.h>
#include "../types/ActionTypes.hpp"
#include "../types/LobbyData.hpp"
#include "ModifyEditorLayer.hpp"
#include "ModifyGameObject.hpp"
#include "ModifyGameManager.hpp"
#include "../utils/Utills.hpp"


using namespace geode::prelude;

void MyGameManager::update(float p0) {
	SteamAPI_RunCallbacks();

	NetManager::get()->update();

	GameManager::update(p0);
}