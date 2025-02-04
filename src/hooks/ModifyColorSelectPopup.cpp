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
        log::info("ClosedColorSelect");
        ColorSelectPopup::closeColorSelect(p0);
    }
};