#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <isteamnetworkingmessages.h>
#include "../ActionTypes.hpp"
#include "ModifyGameManager.hpp"
#include "ModifyEditorLayer.hpp"

using namespace geode::prelude;



GameObject* MyLevelEditorLayer::createObject(int p0, cocos2d::CCPoint p1, bool p2) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

    if (!gameManager->m_fields->m_isInLobby || p2 || m_fields->m_wasDataSent) {
        return LevelEditorLayer::createObject(p0, p1, p2);
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionPlacedObject)},
        {"x", p1.x},
        {"y", p1.y},
        {"ObjID", p0}
    });

    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION).c_str());
    
    return LevelEditorLayer::createObject(p0, p1, p2);
}


void MyLevelEditorLayer::updateLevelFont(int p0) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

    if (gameManager->m_fields->m_isInLobby == false || m_fields->m_wasDataSent) {
        return LevelEditorLayer::updateLevelFont(p0);
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionUpdatedFont)},
        {"FontID", p0}
    });

    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION).c_str());

    return LevelEditorLayer::updateLevelFont(p0);
}
