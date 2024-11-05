#include <Geode/Geode.hpp>

#include "LobbyPopup.hpp"

using namespace geode::prelude;


// specify parameters for the setup function in the Popup<...> template
bool LobbyPopup::setup(std::string const& value) {
    // convenience function provided by Popup
    // for adding/setting a title to the popup
    this->setTitle("Create Lobby:");

    auto label = CCLabelBMFont::create(value.c_str(), "bigFont.fnt");
    m_mainLayer->addChildAtPosition(label, Anchor::Center);

    return true;
}

LobbyPopup* LobbyPopup::create(std::string const& text) {
    auto ret = new LobbyPopup();
    if (ret->initAnchored(240.f, 160.f, text)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
