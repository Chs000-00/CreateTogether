#include <Geode/Geode.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "ModifyEditorLayer.hpp"
#include "HighLevelHeader.hpp"
#include "../networking/NetManager.hpp"
#include "ModifyGameObject.hpp"

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

// TODO: test out shouldDeleteObject
// Note this->m_selectedObject might not have the only selected object, if there is only one selected

class $modify(EditorUI) {

    void onDeleteSelected(CCObject* sender) {

        if (NetManager::getWasDataSent() || !NetManager::getIsInLobby()) {
            EditorUI::onDeleteSelected(sender);
            return;
        }


        IDList idlist;

        auto editor = static_cast<MyLevelEditorLayer*>(this->m_editorLayer);
        for (auto obj : CCArrayExt<MyGameObject*>(this->m_selectedObjects)) {
            editor->m_fields->m_pUniqueIDOfGameObject->removeObjectForKey(obj->m_fields->m_veryUniqueID);
            addStringToIDList(idlist, obj->m_fields->m_veryUniqueID.c_str());
        }

        if (this->m_selectedObject) {
            addStringToIDList(idlist, static_cast<MyGameObject*>(this->m_selectedObject)->m_fields->m_veryUniqueID.c_str());
        }

 
        sendDeleteObjects(idlist);

        EditorUI::onDeleteSelected(sender);
    }

    void moveObject(GameObject* p0, CCPoint p1) {
      
        if (NetManager::getWasDataSent() || !NetManager::getIsInLobby()) {
            EditorUI::moveObject(p0, p1);
            return;
        }

        MyGameObject* betterPlacedGameObject = static_cast<MyGameObject*>(p0);

        sendMoveObjects(betterPlacedGameObject->m_fields->m_veryUniqueID.c_str(), p1);

        EditorUI::moveObject(p0, p1);
    }

    CCArray* pasteObjects(gd::string p0, bool p1, bool p2) {
        CCArray* ret = EditorUI::pasteObjects(p0, p1, p2);
        
        auto objectArr = CCArrayExt<MyGameObject*>(ret);
        auto editorLayer = static_cast<MyLevelEditorLayer*>(this->m_editorLayer);

        for (auto obj : objectArr) {
            auto uid = editorLayer->m_fields->m_userID + "!" + std::to_string(editorLayer->m_fields->m_blocksPlaced);
            obj->m_fields->m_veryUniqueID = uid;
            editorLayer->m_fields->m_blocksPlaced += 1;
            editorLayer->m_fields->m_pUniqueIDOfGameObject->setObject(obj, obj->m_fields->m_veryUniqueID);
        }
        
        if (NetManager::getWasDataSent() || !NetManager::getIsInLobby()) {
            return ret;
        }

        IDList idlist;

        for (auto obj : objectArr) {
            addStringToIDList(idlist, obj->m_fields->m_veryUniqueID.c_str());
        }

        sendPasteObjects(idlist, p0.c_str());
        return ret;
    }
};


