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

        // TODO: Putting this in a pointer as a field of LevelEditorLayer MIGHT be a better idea
        // Then static_casting each time an object is placed
        auto gameManager = GameManager::get();
		auto gameManagerFields = static_cast<MyGameManager*>(gameManager)->m_fields.self();

        if (gameManagerFields->m_isInLobby == false) {
            return LevelEditorLayer::createObject(p0, p1, p2);
        }

        //(auto lobby = lobbyList->begin(); lobby != lobbyList->end(); ++lobby)


        for (auto const& member : gameManagerFields->m_playersInLobby)
        {
            auto msg = SteamNetworkingMessages()->SendMessageToUser(member, );
        } 
        return LevelEditorLayer::createObject(p0, p1, p2);
    }

};