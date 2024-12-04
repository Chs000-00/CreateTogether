#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <isteamnetworkingmessages.h>
#include "uuid_v4.h"
#include "../types/ActionTypes.hpp"
#include "ModifyGameManager.hpp"

using namespace geode::prelude;


class $modify(MyLevelEditorLayer, LevelEditorLayer) {

    struct Fields {
        // Was the data sent from another user?
        bool m_wasDataSent = false;
        CCDictionary* m_pUniqueIDOfGameObject = CCDictionary::create();
        UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
    };


    GameObject* createObject(int p0, cocos2d::CCPoint p1, bool p2);
    void updateLevelFont(int p0);
    bool init(GJGameLevel* p0, bool p1);
    void deleteObject(GameObject *obj);
};
