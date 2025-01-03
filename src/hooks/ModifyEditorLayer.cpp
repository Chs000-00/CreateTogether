#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <uuid_v4.h>
#include "../types/ActionTypes.hpp"
#include "ModifyGameManager.hpp"
#include "ModifyGameObject.hpp"
#include "ModifyEditorLayer.hpp"

using namespace geode::prelude;

bool MyLevelEditorLayer::init(GJGameLevel* p0, bool p1) {

    if (!LevelEditorLayer::init(p0, p1)) {
        return false;
    }

    auto objectArr = CCArrayExt<MyGameObject*>(m_objects);

    for (auto obj : objectArr) {
        obj->m_fields->m_veryUniqueID = this->m_fields->uuidGenerator.getUUID().str();
        this->m_fields->m_pUniqueIDOfGameObject->setObject(obj, obj->m_fields->m_veryUniqueID);
    }

    return true;
}


GameObject* MyLevelEditorLayer::createObject(int p0, cocos2d::CCPoint p1, bool p2) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    MyGameObject* createdGameObject = static_cast<MyGameObject*>(LevelEditorLayer::createObject(p0, p1, p2));
    
    // So for some weird and stupid reason this has to run before the if statement with the return
    auto uid = this->m_fields->uuidGenerator.getUUID().str();


    if (!gameManager->m_fields->m_isInLobby || p2 || m_fields->m_wasDataSent) {
        return createdGameObject;
    }
    
    createdGameObject->m_fields->m_veryUniqueID = uid;

    this->m_fields->m_pUniqueIDOfGameObject->setObject(createdGameObject, uid);
     
    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionPlacedObject)},
        {"x", p1.x},
        {"y", p1.y},
        {"Layer", createdGameObject->m_editorLayer},
        {"ObjID", p0},
        {"ObjectUID", createdGameObject->m_fields->m_veryUniqueID}
    });

     // Extra values which are copied to a new object when created
    if (auto selected = this->m_editorUI->m_selectedObject) {
        object.set("UseExtra", true);
        object.set("Rot", selected->getRotation());
        object.set("HD", selected->m_isHighDetail);
        object.set("NoGlow", selected->m_hasNoGlow);
        object.set("NoEnter", selected->m_isDontEnter);
        object.set("NoFade", selected->m_isDontFade);
        object.set("FlipX", selected->m_isFlipX);
        object.set("FlipY", selected->m_isFlipY);
    }
    
    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));
    
    return createdGameObject;
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

    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));

    return LevelEditorLayer::updateLevelFont(p0);
}

void MyLevelEditorLayer::deleteObject(GameObject *obj) {
    // log::debug("Cleaning up.. Deleting sent object!");
    auto betterGameObject = static_cast<MyGameObject*>(obj);
    EditorUI::get()->deselectObject(obj);
    this->m_fields->m_pUniqueIDOfGameObject->removeObjectForKey(betterGameObject->m_fields->m_veryUniqueID);
    obj->deactivateObject(true);
    LevelEditorLayer::removeObjectFromSection(obj);
    this->removeSpecial(obj);
}
