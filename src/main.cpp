#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "isteamfriends.h"
#include "isteammatchmaking.h"
#include "LobbyPopup.hpp"

using namespace geode::prelude;

class $modify(MyLevelEditorLayer, LevelEditorLayer) {

    struct Fields {
        uint64 m_lobbyId = 0;
        bool m_isInLobby = false;
    };

    bool init(GJGameLevel* p0, bool p1) {
        
        if (!LevelEditorLayer::init(p0, p1)) {
            return false;
        }

        return true;
    }
};

class $modify(MyEditorPauseLayer, EditorPauseLayer) {

	bool init(LevelEditorLayer* p0) {

		if (!EditorPauseLayer::init(p0)) {
			return false;
		}

		auto hostButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("GJ_hammerIcon_001.png", (1.0F), geode::CircleBaseColor::Gray, geode::CircleBaseSize::Small),	
			this,
			menu_selector(MyEditorPauseLayer::onHost)
		);

		auto menu = this->getChildByID("guidelines-menu");
		menu->addChild(hostButton);
		hostButton->setID("host-button"_spr);
		menu->updateLayout();

		return true;
	}

	void onHost(CCObject*) {
		// FLAlertLayer::create("CreateTogether", "Might be hosting!", "OK")->show();
		auto lobby = SteamMatchmaking()->CreateLobby(k_ELobbyTypeFriendsOnly, 16);
		log::info("Creating a lobby...");
		LobbyPopup::create("TEST");
	}

	void inviteFriends() {
		SteamFriends()->ActivateGameOverlay( "friends" );
	}


	void onExitEditor(cocos2d::CCObject* sender) {
		leaveLobby();
	}

	void leaveLobby() {
        // This causes crash! TODO: FIX CRASH!
        // Too lazy to test, but usingstatic_cast<LevelEditorLayer*> might work?
        auto clvlEditor = static_cast<MyLevelEditorLayer*>(m_editorLayer)->m_fields;
        if (clvlEditor->m_isInLobby) {
            SteamMatchmaking()->LeaveLobby(lvlEditor->m_lobbyId);
            log::info("Leaving lobby with ID {}", lvlEditor->m_lobbyId);
        }
        else {
            log::info("Cant leave lobby because not in lobby!")
        }
	}
};
