#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "ModifyPlayLayer.hpp"
#include "ModifyPauseLayer.hpp"
#include "../ModifyGameManager.hpp"

using namespace geode::prelude;

void MyPauseLayer::onQuit(CCObject* sender) {
    auto playLayer = static_cast<MyPlayLayer*>(PlayLayer::get());
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

    if (playLayer->m_fields->m_isFakeLayer) {
        this->getParent()->removeFromParent();
        gameManager->leaveLobby();
        
        // More stuff here im too lazy to implement
    }
    else {
        PauseLayer::onQuit(sender);
    }
}

void MyPauseLayer::onEdit(CCObject* sender) {
    auto playLayer = static_cast<MyPlayLayer*>(PlayLayer::get());
    if (playLayer->m_fields->m_isFakeLayer) {
        this->getParent()->removeFromParent();
    }
    else {
        PauseLayer::onQuit(sender);
    }
}