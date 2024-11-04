#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include "isteamfriends.h"
#include "isteammatchmaking.h"

using namespace geode::prelude;

class $modify(MyEditorPauseLayer, EditorPauseLayer) {

    struct Fields {
        uint64 m_lobbyId = 0;
		bool m_isInLobby = false;
    };

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
		FLAlertLayer::create("CreateTogether", "Might be hosting!", "OK")->show();
		
		auto lobby = SteamMatchmaking()->CreateLobby(k_ELobbyTypeFriendsOnly, 16);
	}

	void inviteFriends() {
		SteamFriends()->ActivateGameOverlay( "friends" );
	}

	void leaveLobby() {
		if (m_fields->m_isInLobby) {
			SteamMatchmaking()->LeaveLobby(m_fields->m_lobbyId);
		}
	}
};