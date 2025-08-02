#include <Geode/Geode.hpp>
#include <Geode/modify/ColorSelectPopup.hpp>
#include "ModifyGameManager.hpp"
#include "ModifyGameObject.hpp"
#include "HighLevelHeader.hpp"

using namespace geode::prelude;

class $modify(ColorSelectPopup) {
    // Popup to select what to edit
    // void colorSelectClosed(GJSpecialColorSelect* p0, int p1) {
    //     log::info("{}", p1);
    //     ColorSelectPopup::colorSelectClosed(p0, p1);
    // }

    // Test this out
    void closeColorSelect(CCObject* p0) {
        if (this->m_gameObject) {

            ColorSelectPopup::closeColorSelect(p0);

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
    }
};