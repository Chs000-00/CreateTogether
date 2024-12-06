#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include "ModifyGameManager.hpp"
#include "ModifyEditorLayer.hpp"
#include "ModifyGameObject.hpp"
#include "ModifyEditorUI.hpp"

// TODO: test out shouldDeleteObject
void MyEditorUI::onDeleteSelected(CCObject* sender) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

    // log::debug("destoryed object");

    // TODO: Add deleting more then 1 object at a time
    auto obj = static_cast<MyGameObject*>(this->m_selectedObject);


    // TODO: Check gamemanager->m_level
    // and use that
    if (auto editor = LevelEditorLayer::get()) {
        auto editorButBetter = static_cast<MyLevelEditorLayer*>(editor);
        editorButBetter->m_fields->m_pUniqueIDOfGameObject->removeObjectForKey(obj->m_fields->m_veryUniqueID.bytes());
        EditorUI::onDeleteSelected(sender);
    }

    if (!gameManager->m_fields->m_isInLobby) {
        EditorUI::onDeleteSelected(sender);
        return;
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionDeletedObject)},
        {"ObjectUID", obj->m_fields->m_veryUniqueID.bytes()}
    });

    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION).c_str());
    EditorUI::onDeleteSelected(sender);
}

void MyEditorUI::transformObject(GameObject* p0, EditCommand p1, bool p2) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    auto betterObject = static_cast<MyGameObject*>(p0);

    log::debug("TRANSFORM OBJECT CALLLED");

    if (!gameManager->m_fields->m_isInLobby || this->m_fields->m_wasDataSent) {
        EditorUI::transformObject(p0, p1, p2);
        return;
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionTransformObject)},
        {"EditCommand", static_cast<int>(p1)},
        {"ObjectUID", betterObject->m_fields->m_veryUniqueID.bytes()}
    });

    EditorUI::transformObject(p0, p1, p2);
}

void MyEditorUI::moveObject(GameObject* p0, CCPoint p1) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    auto betterObject = static_cast<MyGameObject*>(p0);

    if (!gameManager->m_fields->m_isInLobby || this->m_fields->m_wasDataSent) {
        EditorUI::moveObject(p0, p1);
        return;
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionMovedObject)},
        {"x", p1.x},
        {"y", p1.y},
        {"ObjectUID", betterObject->m_fields->m_veryUniqueID.bytes()}
    });

    EditorUI::moveObject(p0, p1);
}