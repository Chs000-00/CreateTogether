#include <Geode/Geode.hpp>
#include <Geode/modify/SelectSettingLayer.hpp>
#include "../ModifyGameManager.hpp"
#include "../../types/ActionTypes.hpp"

using namespace geode::prelude;

class $modify(SelectSettingLayer) {

    void onSelect(CCObject* sender) {
        auto gameManager = static_cast<MyGameManager*>(GameManager::get());

        SelectSettingLayer::onSelect(sender);

        if (!gameManager->m_fields->m_isInLobby) {
            return;
        }
        
        matjson::Value object;

        if (this->m_type == SelectSettingType::StartingSpeed) {
            object = matjson::makeObject({
                {"Type", static_cast<int>(eOptionSpeedChanged)},
                {"Speed", this->getSelectedValue()}
            });
        }
        else {
            object = matjson::makeObject({
                {"Type", static_cast<int>(eOptionGameModeChanged)},
                {"GameMode", this->getSelectedValue()}
            });   
        }


        gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));
    }
};
