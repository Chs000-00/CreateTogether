#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include "../ActionTypes.hpp"
#include "ModifyEditorLayer.hpp"
#include "ModifyGameManager.hpp"
#include "ModifyGameObject.hpp"

using namespace geode::prelude;

bool MyGameObject::initWithTexture(cocos2d::CCTexture2D* p0) {
    if (!GameObject::initWithTexture(p0)) {
        return false;
    }

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());
    if (!level) {
        level = static_cast<MyLevelEditorLayer*>(gameManager->m_fields->m_level);
    }
    else {
        return true;
    }

    // Hopefully this is called before LevelEditorLayer::createObject
    // this->m_fields->m_veryUniqueID = level->m_fields->m_veryUniqueIDTotal;
    
    return true;
}


void MyGameObject::destroyObject() {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());


    // TODO: Check gamemanager->m_level
    // and use that
    if (auto editor = LevelEditorLayer::get()) {
        auto editorButBetter = static_cast<MyLevelEditorLayer*>(editor);
        editorButBetter->m_fields->m_pUniqueIDOfGameObject->removeObjectForKey(this->m_fields->m_veryUniqueID);
        GameObject::destroyObject();
    }

    if (!gameManager->m_fields->m_isInLobby || m_fields->m_wasDataSent) {
        GameObject::destroyObject();
        return;
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionDeletedObject)},
        {"ObjectUID", this->m_fields->m_veryUniqueID}
    });

    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION).c_str());

    
    GameObject::destroyObject();
}

void MyGameObject::setPosition(const CCPoint& p0) {


    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

    if (!gameManager->m_fields->m_isInLobby || m_fields->m_wasDataSent || LevelEditorLayer::get()) {
        GameObject::setPosition(p0);
        return;
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionMovedObject)},
        {"ObjectUID", this->m_fields->m_veryUniqueID},
        {"x", p0.x},
        {"y", p0.y}
    });

    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION).c_str());

    GameObject::setPosition(p0);
    
}