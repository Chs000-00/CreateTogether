#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/LoadingSpinner.hpp>

using namespace geode::prelude;
// specify parameters for the setup function in the Popup<...> template
class WaitingForHostPopup : public geode::Popup<> {
protected:
    bool setup() override {
        // convenience function provided by Popup
        // for adding/setting a title to the popup
        this->setTitle("Waiting for host.");

        auto label = CCLabelBMFont::create("Waiting for host to enter the editor.", "bigFont.fnt");
        m_mainLayer->addChildAtPosition(label, Anchor::Center);

        auto spinner = LoadingSpinner::create(100);
        spinner->setID("loading-spinner");
        m_mainLayer->addChildAtPosition(spinner, Anchor::Center);

        return true;
    }

public:
    static WaitingForHostPopup* create() {
        auto ret = new WaitingForHostPopup();
        if (ret->initAnchored(240.f, 160.f)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};
