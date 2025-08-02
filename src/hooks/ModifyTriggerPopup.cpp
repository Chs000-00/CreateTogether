#include <Geode/Geode.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>
#include "ModifyGameManager.hpp"
#include "ModifyGameObject.hpp"
#include "HighLevelHeader.hpp"

using namespace geode::prelude;

// Looks like this function is called anytime a toggle setting gets changed, like "Start as duel"
class $modify(MySetupTriggerPopup, SetupTriggerPopup) {
    void onClose(cocos2d::CCObject* sender) {
        SetupTriggerPopup::onClose(sender);

        // log::info("closedPopup");


        if (!NetManager::getIsInLobby()) {
            return;
        }

        // TODO: This might break if multiple gameObjects are selected
        // auto ret = getJsonFromGameObject(this->m_gameObject);

        auto editorUI = EditorUI::get();

        if (!editorUI) {
            log::warn("Error: EditorUI::get() returned nullptr");
            return;
        }

        cocos2d::CCArray* editObjs;

        if (this->m_gameObject) {
            editObjs = CCArray::createWithObject(this->m_gameObject);
        } 
        else if (this->m_gameObjects) {
            editObjs = this->m_gameObjects;
        } 
        else {
            log::info("No suitable gameObject found!");
            return;
        }

        // TODO: Figure out args
        std::string objString = editorUI->copyObjects(editObjs, false, false);

        IDList idlist;

        for (auto obj : CCArrayExt<MyGameObject*>(editObjs)) {
            addStringToIDList(idlist, obj->m_fields->m_veryUniqueID.c_str());
        }

        sendModifyObjects(idlist, objString.c_str());
    }
};