#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include "../networking/NetManager.hpp"
#include "../networking/SteamManager.hpp"


using namespace geode::prelude;


class $modify(MyGameManager, GameManager) {
	
    struct Fields {
		NetManager* m_netManager = new NetManager;
		SteamManager* m_callbackManager = new SteamManager;
    };

	void update(float p0);
};
