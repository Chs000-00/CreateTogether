#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include "../ActionTypes.hpp"
#include "ModifyEditorLayer.hpp"
#include "ModifyGameManager.hpp"
#include "ModifyGameObject.hpp"

using namespace geode::prelude;

void MyGameObject::destroyObject() {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());


    if (auto editor = LevelEditorLayer::get()) {
        auto editorButBetter = static_cast<MyLevelEditorLayer*>(editor);
        editorButBetter->m_fields->m_pUniqueIDOfGameObject->removeObjectForKey(this->m_uniqueID);
        GameObject::destroyObject();
        return;
    }

    if (!gameManager->m_fields->m_isInLobby || m_fields->m_wasDataSent) {
        GameObject::destroyObject();
        return;
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionDeletedObject)},
        {"ObjectUID", this->m_uniqueID}
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
        {"ObjectUID", this->m_uniqueID},
        {"x", p0.x},
        {"y", p0.y}
    });

    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION).c_str());

    GameObject::setPosition(p0);
    
}