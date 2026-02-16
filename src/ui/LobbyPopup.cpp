#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameManager.hpp>

#ifdef STEAMWORKS
    #include <isteammatchmaking.h>
#endif

#include "LobbyPopup.hpp"
#include "SteamManager.hpp"
#include "NetManager.hpp"
#include "../config.hpp"

using namespace geode::prelude;

#ifdef STEAMWORKS

// specify parameters for the setup function in the Popup<...> template
bool LobbyPopup::setup(EPopupType type) {

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
        publicToggle->toggle(!NetManager::get()->m_options.isPrivate);

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
    netManager->m_options.isPrivate = !netManager->m_options.isPrivate;
    // sender->toggle(netManager->m_options.isPrivate);

    log::debug("Setting isPublic to {}.", netManager->m_options.isPrivate);

    if (NetManager::getIsInLobby()) {
        
        // TODO: Check if this code works

        // if (netManager->m_options.canFriendsJoin) {
        //     if (netManager->m_options.isPrivate) {
        //         steamCallbacks->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 16);
        //     }
        //     else {
        //         steamCallbacks->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypeFriendsOnly, 16);
        //     }
        // }
        // else {
        //     if (!netManager->m_options.isPrivate) {
        //         steamCallbacks->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypePrivate, 16);
        //     }    
        // }

    }
}


void LobbyPopup::startHosting(CCObject* sender) {

    auto netManager = NetManager::get();


    this->onClose(nullptr);


    if (netManager->m_options.canFriendsJoin) {
        if (netManager->m_options.isPrivate) {
            log::info("Creating public lobby.");
            SteamManager::get()->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, MAX_USERS);
        }
        else {
            log::info("Creating friends only lobby.");
            SteamManager::get()->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypeFriendsOnly, MAX_USERS);
        }
    }
    else {
        if (!netManager->m_options.isPrivate) {
            log::info("Creating private lobby.");
            SteamManager::get()->m_lobbyCreated = SteamMatchmaking()->CreateLobby(k_ELobbyTypePrivate, MAX_USERS);
        }    
    }

    netManager->m_isHost = true;
    SteamManager::get()->m_isInLobbyCallResult.Set(SteamManager::get()->m_lobbyCreated, SteamManager::get(), &SteamManager::onLobbyCreated);
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

    log::info("Creating popup");

    if (ret->initAnchored(300.f, 200.f, type)) {
        ret->autorelease();
        return ret;
    }


    delete ret;
    return nullptr;
}

