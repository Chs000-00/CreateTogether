#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameManager.hpp>
#include <isteammatchmaking.h>
#include "LobbyPopup.hpp"
#include "../hooks/ModifyGameManager.hpp"

using namespace geode::prelude;


// specify parameters for the setup function in the Popup<...> template
bool LobbyPopup::setup(EPopupType type) {
    // convenience function provided by Popup
    // for adding/setting a title to the popup

    switch (type) {
        case eLobbyHostPopup: {
            this->setTitle("Create Lobby:");

            auto startHostingButton = CCMenuItemSpriteExtra::create(
                ButtonSprite::create("Host"), 
                this, 
                menu_selector(LobbyPopup::startHosting)
            );

            std::function<void(CCMenuItemToggler*)> func = LobbyPopup::onPublicToggle;

            auto publicToggle = CCMenuItemExt::createTogglerWithStandardSprites(
                1.0f,
            );

            this->m_buttonMenu->addChildAtPosition(startHostingButton, Anchor::Bottom, {0, 20});

            break;
        }


        case eLobbyHostingPopup: {
            this->setTitle("Lobby settings:");

            auto inviteFriendsButton = CCMenuItemSpriteExtra::create(
                ButtonSprite::create("Invite"), 
                this, 
                menu_selector(LobbyPopup::inviteFriends)
            );

            this->m_buttonMenu->addChildAtPosition(inviteFriendsButton, Anchor::Bottom, {0, 20});

            break;
        }

    }

    return true;
}

void LobbyPopup::onPublicToggle(CCMenuItemToggler* sender) {

}


void LobbyPopup::startHosting(CCObject* sender) {

    auto gameManagerCast = static_cast<MyGameManager*>(GameManager::get());
    auto gameManagerFields = gameManagerCast->m_fields.self();

    this->onClose(nullptr);

    //gameManagerFields->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypeFriendsOnly, 16);
    gameManagerFields->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 16);
    gameManagerFields->m_isHost = true;
    gameManagerFields->m_isInLobbyCallResult.Set(gameManagerFields->m_lobbyCreated, gameManagerCast, &MyGameManager::onLobbyCreated);

}

void LobbyPopup::inviteFriends(CCObject* sender) {
    auto gameManagerFields = static_cast<MyGameManager*>(GameManager::get())->m_fields.self();
    SteamFriends()->ActivateGameOverlayInviteDialog(gameManagerFields->m_lobbyId);
}

LobbyPopup* LobbyPopup::create(EPopupType type) {
    auto ret = new LobbyPopup();

    log::info("Creating popup...");

    if (ret->initAnchored(300.f, 200.f, type)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
