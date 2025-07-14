#include <Geode/Geode.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "ModifyEditorLayer.hpp"
#include "../networking/HighLevelHeader.hpp"
#include "../networking/NetManager.hpp"
#include "ModifyGameObject.hpp"


using namespace geode::prelude;

// TODO: test out shouldDeleteObject
// Note this->m_selectedObject might not have the only selected object, if there is only one selected

class $modify(EditorUI) {

    void onDeleteSelected(CCObject* sender) {

        if (NetManager::getWasDataSent() || NetManager::getIsInLobby()) {
            EditorUI::onDeleteSelected(sender);
            return;
        }

        IDList idlist;

        auto editor = static_cast<MyLevelEditorLayer*>(this->m_editorLayer);
        for (auto obj : CCArrayExt<MyGameObject*>(this->m_selectedObjects)) {
            editor->m_fields->m_pUniqueIDOfGameObject->removeObjectForKey(obj->m_fields->m_veryUniqueID);
            addStringToIDList(idlist, obj->m_fields->m_veryUniqueID.c_str());
        }

        sendDeleteObjects(idlist);

        EditorUI::onDeleteSelected(sender);
    }

};