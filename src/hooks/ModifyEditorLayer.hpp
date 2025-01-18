#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <isteamnetworkingmessages.h>
#include "../types/ActionTypes.hpp"
#include "ModifyGameManager.hpp"

using namespace geode::prelude;
class $modify(MyLevelEditorLayer, LevelEditorLayer) {

    struct Fields {
        // Was the data sent from another user?
        bool m_wasDataSent = false;
        unsigned int m_blocksPlaced = 0;
        CCDictionary* m_pUniqueIDOfGameObject = CCDictionary::create();
    };


    GameObject* createObject(int p0, cocos2d::CCPoint p1, bool p2);
    void updateLevelFont(int p0);
    bool init(GJGameLevel* p0, bool p1);
    void deleteObject(GameObject* obj);
};