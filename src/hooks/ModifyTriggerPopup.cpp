#include <Geode/Geode.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>
#include "ModifyGameManager.hpp"
#include "ModifyGameObject.hpp"
#include "../types/ActionTypes.hpp"
#include "../utils/Utills.hpp"

using namespace geode::prelude;

// TODO: Implement
// Looks like this function is called anytime a toggle setting gets changed, like "Start as duel"
class $modify(MySetupTriggerPopup, SetupTriggerPopup) {
    void onClose(cocos2d::CCObject* sender) {
        SetupTriggerPopup::onClose(sender);

        auto gameManager = static_cast<MyGameManager*>(GameManager::get());
        log::info("closedPopup");


        if (!gameManager->m_fields->m_isInLobby) {
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
            log::warn("Warning: no suitable gameObject found!");
            return;
        }

        std::string objString = editorUI->copyObjects(editObjs, false, false);

        matjson::Value object = matjson::makeObject({
            {"Type", static_cast<int>(eActionModifiedObject)},
            {"ObjectString", objString}
        });

        matjson::Value eUUIDs = matjson::Value::array();

        auto objectArr = CCArrayExt<MyGameObject*>(editObjs);

        for (auto obj : objectArr) {
            eUUIDs.push(obj->m_fields->m_veryUniqueID);
        }

	    object["EditUUIDs"] = eUUIDs;

        gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));

    }
};
