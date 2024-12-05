#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include "ModifyGameManager.hpp"
#include "ModifyEditorLayer.hpp"
#include "ModifyGameObject.hpp"
#include "ModifyEditorUI.hpp"


void MyEditorUI::onDeleteSelected(CCObject* sender) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

    log::debug("destoryed object");

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