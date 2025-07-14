#include "../HighLevelHeader.hpp"
#include "SharedHighLevelHeaders.hpp"
#include "../../hooks/ModifyEditorLayer.hpp"
#include "../../hooks/ModifyGameObject.hpp"
#include <Geode/binding/EditorUI.hpp>


// TODO: Verifier stuff

Result<uint8_t> recvCreateObjects(const CTSerialize::CreateObjects* msg) {
    auto minObj = msg->obj();
    cocos2d::CCPoint gameObjectPos = {static_cast<float>(minObj->pos()->x()), static_cast<float>(minObj->pos()->y())};

    // TODO: nullptr check this
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());
    // TODO: Figure if a race condition is possible
    NetManager::get()->m_wasDataSent = true;
    GameObject* placedGameObject = level->createObject(minObj->objID(), gameObjectPos, false);
    NetManager::get()->m_wasDataSent = false;

    placedGameObject->setRotation(minObj->rotation());
    placedGameObject->m_isHighDetail = minObj->isHighDetail();
    placedGameObject->m_hasNoGlow = minObj->noGlow();
    placedGameObject->m_isDontEnter = minObj->noEnter();

    if(auto flip = minObj->flip()) {
        placedGameObject->setFlipX(flip->flipX());
        placedGameObject->setFlipY(flip->flipY());
    }

    MyGameObject* betterPlacedGameObject = static_cast<MyGameObject*>(placedGameObject);
    betterPlacedGameObject->m_fields->m_veryUniqueID = minObj->uniqueID()->str();
    level->m_fields->m_pUniqueIDOfGameObject->setObject(placedGameObject, minObj->uniqueID()->str());

    return Ok(0);
}

Result<uint8_t> recvDeleteObjects(const CTSerialize::DeleteObjects* msg) {
    auto idlist = msg->uniqueIDList();

    auto editor = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());
    for (auto i = 0; i < idlist->Length(); i++) {
        log::info("c1");
        if (auto dObj = editor->m_fields->m_pUniqueIDOfGameObject->objectForKey(idlist->Get(i)->str())) {
            editor->deleteObject(static_cast<GameObject*>(dObj));
        }
    }
    return Ok(0);
}

Result<uint8_t> recvMoveObjects(const CTSerialize::MoveObjects* msg) {
    auto id = msg->uniqueID();
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());
    cocos2d::CCPoint offsetPos = {static_cast<float>(msg->positionOffset()->x()), static_cast<float>(msg->positionOffset()->y())};


    if (auto dObj = level->m_fields->m_pUniqueIDOfGameObject->objectForKey(id->str())) {
        NetManager::get()->m_wasDataSent = true;
        level->m_editorUI->moveObject(static_cast<GameObject*>(dObj), offsetPos);
        NetManager::get()->m_wasDataSent = false;
    }
}