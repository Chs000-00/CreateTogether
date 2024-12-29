#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include "ModifyGameManager.hpp"
#include "ModifyEditorLayer.hpp"
#include "ModifyGameObject.hpp"
#include "ModifyEditorUI.hpp"

using namespace geode::prelude;


// TODO: test out shouldDeleteObject
void MyEditorUI::onDeleteSelected(CCObject* sender) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

    log::debug("destoryed selected object");

    // log::debug("Obj with uuid {}", obj->m_fields->m_veryUniqueID.str());

    if (!gameManager->m_fields->m_isInLobby || this->m_selectedObjects || this->m_fields->m_wasDataSent) {
        EditorUI::onDeleteSelected(sender);
        return;
    }

    auto editUUID = removeSelectedObjectsWithMatjson();

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionDeletedObject)},
        {"EditUUIDs", editUUID},
    });

    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION).c_str());
    EditorUI::onDeleteSelected(sender);
}

void MyEditorUI::removeSelectedObjects() {
    auto editor = static_cast<MyLevelEditorLayer*>(this->m_editorLayer);

    for (auto obj : CCArrayExt<MyGameObject*>(this->m_selectedObjects)) {
        editor->m_fields->m_pUniqueIDOfGameObject->removeObjectForKey(obj->m_fields->m_veryUniqueID.bytes());
    }
}

matjson::Value MyEditorUI::removeSelectedObjectsWithMatjson() {
    auto editor = static_cast<MyLevelEditorLayer*>(this->m_editorLayer);
    auto ret = matjson::Value::array();
    for (auto obj : CCArrayExt<MyGameObject*>(this->m_selectedObjects)) {
        editor->m_fields->m_pUniqueIDOfGameObject->removeObjectForKey(obj->m_fields->m_veryUniqueID.bytes());
        ret.push(obj->m_fields->m_veryUniqueID.bytes());
    }
    return ret;
}

void MyEditorUI::transformObject(GameObject* p0, EditCommand p1, bool p2) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    auto betterObject = static_cast<MyGameObject*>(p0);

    log::debug("TransformObject called with command {}", fmt::underlying(p1));

    if (!gameManager->m_fields->m_isInLobby || this->m_fields->m_wasDataSent) {
        log::debug("TransformObjCalling...");
        EditorUI::transformObject(p0, p1, p2);
        return;
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionTransformObject)},
        {"EditCommand", static_cast<int>(p1)},
        {"ObjectUID", betterObject->m_fields->m_veryUniqueID.bytes()}
    });

    log::info("Sending data: {}", object.dump(matjson::NO_INDENTATION));
    
    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION).c_str());

    EditorUI::transformObject(p0, p1, p2);
}

void MyEditorUI::moveObject(GameObject* p0, CCPoint p1) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    auto betterObject = static_cast<MyGameObject*>(p0);


    log::info("MoveObjectCalled");

    if (!gameManager->m_fields->m_isInLobby || this->m_fields->m_wasDataSent || !this->m_fields->m_loadingFinished) {
        EditorUI::moveObject(p0, p1);
        return;
    }


    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionMovedObject)},
        {"x", p1.x},
        {"y", p1.y},
        {"ObjectUID", betterObject->m_fields->m_veryUniqueID.bytes()}
    });

    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION).c_str());

    EditorUI::moveObject(p0, p1);
}

bool MyEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) {
        return false;
    }
    this->m_fields->m_loadingFinished = true;
    return true;
}