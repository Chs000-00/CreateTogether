#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

enum EPopupType {
    eLobbyHostPopup,
    eLobbyHostingPopup,
    eLobbyJoinedUserPopup
};

// specify parameters for the setup function in the Popup<...> template
class LobbyPopup : public geode::Popup<EPopupType> {
private:
    bool setup(EPopupType type) override;

public:
    static LobbyPopup* create(EPopupType type);
    void startHosting(CCObject* sender);
    void inviteFriends(CCObject* sender);
    void onPublicToggle(CCMenuItemToggler* sender);
};
