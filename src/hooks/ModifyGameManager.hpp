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
#include "../config.hpp"

#ifdef USE_TEST_SERVER
	#include <WinSock2.h>
	#include "../types/PlaceboMsg.hpp"
#endif

using namespace geode::prelude;


class $modify(MyGameManager, GameManager) {
    struct Fields {
		CallbackManager* m_callbackManager;
		NetworkManager* m_networkManager;
    };

	void update(float p0);
};
