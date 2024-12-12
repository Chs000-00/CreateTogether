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

    if (type != eLobbyJoinedUserPopup) {
        std::function<void (CCMenuItemToggler *)> pfunc = std::bind(&LobbyPopup::onPublicToggle, this, std::placeholders::_1);

        auto checkMarkMenu = CCMenu::create();
        checkMarkMenu->setLayout(RowLayout::create());
        checkMarkMenu->setContentSize(this->getContentSize());

        auto publicText = CCLabelBMFont::create("Public Lobby", "bigFont.fnt");
        auto publicToggle = CCMenuItemExt::createTogglerWithStandardSprites(
            1.0f,
            pfunc
        );

        checkMarkMenu->addChild(publicText);
        checkMarkMenu->addChild(publicToggle);
        checkMarkMenu->updateLayout();
        checkMarkMenu->setScale(0.5f);
        this->m_buttonMenu->addChildAtPosition(checkMarkMenu, Anchor::Center);

    }

    return true;
}

void LobbyPopup::onPublicToggle(CCMenuItemToggler* sender) {
    this->isServerPublic = sender->m_toggled; 
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    if (gameManager->m_fields->m_isInLobby) {
        log::debug("Setting isPublic to {}", sender->m_toggled);
        // TODO: Check if this code works

        if (this->isServerPublic) {
            SteamMatchmaking()->SetLobbyType(gameManager->m_fields->m_lobbyId, k_ELobbyTypePublic);
        }
        else {
            SteamMatchmaking()->SetLobbyType(gameManager->m_fields->m_lobbyId, k_ELobbyTypeFriendsOnly);
        }
    }
}


void LobbyPopup::startHosting(CCObject* sender) {

    auto gameManagerCast = static_cast<MyGameManager*>(GameManager::get());
    auto gameManagerFields = gameManagerCast->m_fields.self();

    this->onClose(nullptr);

    if (this->isServerPublic) {
        gameManagerFields->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 16);
    }
    else {
        gameManagerFields->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypeFriendsOnly, 16);
    }

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
