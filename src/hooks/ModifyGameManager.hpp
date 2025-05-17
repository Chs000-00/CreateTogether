#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include "../networking/NetManager.hpp"
#include "../networking/SteamCallbacks.hpp"


using namespace geode::prelude;


class $modify(MyGameManager, GameManager) {
	
    struct Fields {
		SteamCallbacks* m_callbackManager;
		NetManager* m_netManager;
    };

	void update(float p0);
};
