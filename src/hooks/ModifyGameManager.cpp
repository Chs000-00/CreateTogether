#include "../config.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include "ModifyGameManager.hpp"

#include <isteamnetworkingsockets.h>


using namespace geode::prelude;

void MyGameManager::update(float p0) {

	#ifdef STEAMWORKS
		SteamAPI_RunCallbacks();
	#else
		SteamNetworkingSockets()->RunCallbacks();
	#endif

	NetManager::get()->update();

	GameManager::update(p0);
}