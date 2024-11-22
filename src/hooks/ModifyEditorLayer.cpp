#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <isteamnetworkingmessages.h>
#include "ModifyGameManager.hpp"

using namespace geode::prelude;


class $modify(MyLevelEditorLayer, LevelEditorLayer) {

    // void addSpecial(GameObject* p0) {
    //     log::info("AddSpecial called with gameobject uid {}", p0->m_uniqueID);
    //     LevelEditorLayer::addSpecial(p0);
    // }

    GameObject* createObject(int p0, cocos2d::CCPoint p1, bool p2) {
        log::info("createObject called with args {} : {}, {} : {}", p0, p1.x, p1.y, p2);

		auto gameManager = static_cast<MyGameManager*>(GameManager::get());

        if (gameManager->m_fields->m_isInLobby == false || p2) {
            return LevelEditorLayer::createObject(p0, p1, p2);
        }

        matjson::Value object = matjson::makeObject({
            {"Type", "makeObject"},
            {"x", p1.x},
            {"y", p1.y},
            {"ObjID", p0}
        });

        gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION).c_str());
 
        return LevelEditorLayer::createObject(p0, p1, p2);
    }

};