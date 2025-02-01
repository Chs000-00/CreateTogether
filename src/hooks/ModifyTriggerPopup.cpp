#include <Geode/Geode.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>
#include "ModifyGameManager.hpp"
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
        auto ret = getJsonFromGameObject(this->m_gameObject);

        gameManager->sendDataToMembers(ret.dump(matjson::NO_INDENTATION));

    }
};
