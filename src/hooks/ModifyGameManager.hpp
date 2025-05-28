#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include "../networking/NetManager.hpp"
#include "../networking/SteamCallbacks.hpp"


using namespace geode::prelude;


class $modify(MyGameManager, GameManager) {
	
    struct Fields {
		NetManager* m_netManager;
		SteamCallbacks* m_callbackManager;
    };

	void update(float p0);
};
