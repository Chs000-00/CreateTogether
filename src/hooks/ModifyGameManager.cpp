#include "../config.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include "ModifyGameManager.hpp"


using namespace geode::prelude;

void MyGameManager::update(float p0) {
	NetManager::get()->update();

	GameManager::update(p0);
}