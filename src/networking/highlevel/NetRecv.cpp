#include "../HighLevelHeader.hpp"
#include "SharedHighLevelHeaders.hpp"
#include "../../hooks/ModifyEditorLayer.hpp"
#include "../../hooks/ModifyGameObject.hpp"


// TODO: Verifier stuff

Result<uint8_t> recvCreateObjects(const CTSerialize::CreateObjects* msg) {
    auto minObj = msg->obj();
    cocos2d::CCPoint gameObjectPos = {static_cast<float>(minObj->pos()->x()), static_cast<float>(minObj->pos()->y())};

    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());
    // TODO: Figure if a race condition is possible
    level->m_fields->m_wasDataSent = true;
    GameObject* placedGameObject = level->createObject(minObj->objID(), gameObjectPos, false);
    level->m_fields->m_wasDataSent = false;

    placedGameObject->setRotation(minObj->rotation());
    placedGameObject->m_isHighDetail = minObj->isHighDetail();
    placedGameObject->m_hasNoGlow = minObj->noGlow();
    placedGameObject->m_isDontEnter = minObj->noEnter();
    placedGameObject->setFlipX(minObj->flip()->flipX());
    placedGameObject->setFlipY(minObj->flip()->flipY());

    MyGameObject* betterPlacedGameObject = static_cast<MyGameObject*>(placedGameObject);
    betterPlacedGameObject->m_fields->m_veryUniqueID = minObj->uniqueID()->str();
    level->m_fields->m_pUniqueIDOfGameObject->setObject(placedGameObject, minObj->uniqueID()->str());

    return Ok(0);
}