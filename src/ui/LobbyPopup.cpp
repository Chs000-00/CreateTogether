#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameManager.hpp>

#ifdef STEAMWORKS
    #include <isteammatchmaking.h>
#endif

#include "LobbyPopup.hpp"
#include "../hooks/ModifyGameManager.hpp"
#include "../networking/NetManager.hpp"

using namespace geode::prelude;

#ifdef STEAMWORKS

// specify parameters for the setup function in the Popup<...> template
bool LobbyPopup::setup(EPopupType type) {
    // convenience function provided by Popup
    // for adding/setting a title to the popup

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

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


        // TODO: Fix this reseting or something idfk
        publicToggle->toggle(gameManager);

        checkMarkMenu->addChild(publicText);
        checkMarkMenu->addChild(publicToggle);
        checkMarkMenu->updateLayout();
        checkMarkMenu->setScale(0.5f);
        this->m_buttonMenu->addChildAtPosition(checkMarkMenu, Anchor::Center);

    }

    return true;
}

void LobbyPopup::onPublicToggle(CCMenuItemToggler* sender) {
    auto netManager = NetManager::get();
    netManager->m_options.isPrivate = sender->m_toggled; 

    auto steamCallbacks = static_cast<MyGameManager*>(GameManager::get())->m_fields->m_callbackManager;

    if (netManager->m_isInLobby) {
        log::debug("Setting isPublic to {}", sender->m_toggled);
        // TODO: Check if this code works

        if (netManager->m_options.canFriendsJoin) {
            if (netManager->m_options.isPrivate) {
                steamCallbacks->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 16);
            }
            else {
                steamCallbacks->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypeFriendsOnly, 16);
            }
        }
        else {
            if (!netManager->m_options.isPrivate) {
                steamCallbacks->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypePrivate, 16);
            }    
        }

    }
}


void LobbyPopup::startHosting(CCObject* sender) {

    auto netManager = NetManager::get();
    auto steamCallbacks = static_cast<MyGameManager*>(GameManager::get())->m_fields->m_callbackManager;



    this->onClose(nullptr);


    if (netManager->m_options.canFriendsJoin) {
        if (netManager->m_options.isPrivate) {
            steamCallbacks->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 16);
        }
        else {
            steamCallbacks->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypeFriendsOnly, 16);
        }
    }
    else {
        if (!netManager->m_options.isPrivate) {
            steamCallbacks->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypePrivate, 16);
        }    
    }

    netManager->m_isHost = true;
    steamCallbacks->m_isInLobbyCallResult.Set(steamCallbacks->m_lobbyCreated, steamCallbacks, &SteamManager::onLobbyCreated);
}

void LobbyPopup::inviteFriends(CCObject* sender) {
    auto netManager = NetManager::get();
    SteamFriends()->ActivateGameOverlayInviteDialog(netManager->m_lobbyId);
}


#else

bool LobbyPopup::setup(EPopupType type) {
    this->setTitle("Unavailable due to debug mode");
    return true;
}

#endif

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

