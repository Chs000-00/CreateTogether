#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <isteamnetworkingmessages.h>
#include "../types/ActionTypes.hpp"
#include "ModifyGameManager.hpp"
#include <isteamuser.h>

using namespace geode::prelude;
class $modify(MyLevelEditorLayer, LevelEditorLayer) {

    struct Fields {
        // Was the data sent from another user?
        bool m_wasDataSent = false;
        unsigned int m_blocksPlaced = 0;


        bool m_loadingFinished = false;
        
        #ifndef USE_TEST_SERVER
        uint64 m_userID = SteamUser()->GetSteamID().ConvertToUint64();
        #else
        uint64 m_userID;
        #endif

        PlayLayer* m_fakePlayLayer = nullptr;

        // There is a weird crash happening when using setObject
        // Looks like it might be happening due to it being an autorelease object?
        Ref<CCDictionary> m_pUniqueIDOfGameObject = nullptr;
    };

    // Fakes a play layer so I dont have to deal with weird networking
    // And sync stuff weirdly
    void createFakePlayLayer();

    GameObject* createObject(int p0, cocos2d::CCPoint p1, bool p2);
    void updateLevelFont(int p0);
    bool init(GJGameLevel* p0, bool p1);
    void deleteObject(GameObject* obj);
    void addToGroup(GameObject* p0, int p1, bool p2);
    void removeFromGroup(GameObject* p0, int p1);
};