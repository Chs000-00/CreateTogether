#include <Geode/Geode.hpp>
#include <Geode/modify/ColorSelectPopup.hpp>
#include "ModifyGameManager.hpp"
#include "ModifyGameObject.hpp"
#include "../types/ActionTypes.hpp"

using namespace geode::prelude;

class $modify(ColorSelectPopup) {
    // Popup to select what to edit
    // void colorSelectClosed(GJSpecialColorSelect* p0, int p1) {
    //     log::info("{}", p1);
    //     ColorSelectPopup::colorSelectClosed(p0, p1);
    // }

    // Test this out
    void closeColorSelect(CCObject* p0) {
        log::info("ClosedColorSelectClosed");

        if (this->m_colorTrigger) {
            log::info("Color Trigger");
        }
        if (this->m_gameObject) {
            log::info("Oh there is a game object!");
        }

        ColorSelectPopup::closeColorSelect(p0);
    }
};