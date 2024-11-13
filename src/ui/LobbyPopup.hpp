#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// specify parameters for the setup function in the Popup<...> template
class LobbyPopup : public geode::Popup<> {
private:
    bool setup() override;

public:
    static LobbyPopup* create();
    void startHosting(CCObject* sender);
    void inviteFriends(CCObject* sender);
};
