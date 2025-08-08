#include <Geode/Geode.hpp>
#include <Geode/modify/SelectSettingLayer.hpp>
#include "HighLevelHeader.hpp"

using namespace geode::prelude;

class $modify(SelectSettingLayer) {
    void onSelect(CCObject* sender) {
        SelectSettingLayer::onSelect(sender);

        if (!NetManager::getIsInLobby()) {
            return;
        }

        // TODO: Test to see if this works with other SelectSettingTypes
        if (this->m_type == SelectSettingType::StartingSpeed) {
            sendSpeedChange(this->getSelectedValue());
        }
        else {
            sendGameModeChange(this->getSelectedValue());
        }
    }
};