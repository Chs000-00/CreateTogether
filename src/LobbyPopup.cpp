#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "LobbyPopup.hpp"
#include "isteammatchmaking.h"
#include "ModifyEditorLayer.hpp"

using namespace geode::prelude;


// specify parameters for the setup function in the Popup<...> template
bool LobbyPopup::setup() {
    // convenience function provided by Popup
    // for adding/setting a title to the popup
    this->setTitle("Create Lobby:");

    auto startHostingButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Host"), 
        this, 
        menu_selector(LobbyPopup::startHosting)
    );

    auto inviteFriendsButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Invite"), 
        this, 
        menu_selector(LobbyPopup::inviteFriends)
    );

    this->m_buttonMenu->addChildAtPosition(startHostingButton, Anchor::Bottom, {20, 20});
    this->m_buttonMenu->addChildAtPosition(inviteFriendsButton, Anchor::BottomLeft, {0, 20});

    return true;
}


void LobbyPopup::startHosting(CCObject* sender) {
    if (auto* lvlEditorLayer = LevelEditorLayer::get()) {

        auto lvlEditorCast = static_cast<MyLevelEditorLayer*>(lvlEditorLayer);
        auto lvlEditorFields = lvlEditorCast->m_fields.self();

        lvlEditorFields->m_lobby = SteamMatchmaking()->CreateLobby(k_ELobbyTypeFriendsOnly, 16);
        lvlEditorFields->m_isInLobbyCallResult.Set(lvlEditorFields->m_lobby, lvlEditorCast, &MyLevelEditorLayer::onLobbyCreated);
    }
}

void LobbyPopup::inviteFriends(CCObject* sender) {
    SteamFriends()->ActivateGameOverlay( "friends" );
}

LobbyPopup* LobbyPopup::create() {
    auto ret = new LobbyPopup();

    log::info("Creating popup...");

    if (ret->initAnchored(300.f, 200.f)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
