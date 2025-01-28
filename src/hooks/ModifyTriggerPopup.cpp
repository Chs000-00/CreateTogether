#include <Geode/Geode.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>
#include "ModifyGameManager.hpp"
#include "../types/ActionTypes.hpp"

using namespace geode::prelude;

// TODO: Implement
// Looks like this function is called anytime a toggle setting gets changed, like "Start as duel"
class $modify(MySetupTriggerPopup, SetupTriggerPopup) {
    void onClose(cocos2d::CCObject* sender) {
        SetupTriggerPopup::oonClose(sender);
    }
};
