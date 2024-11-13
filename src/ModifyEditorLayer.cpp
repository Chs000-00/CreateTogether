#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <isteamfriends.h>
#include <isteammatchmaking.h>
#include "ui/LobbyPopup.hpp"
#include "ModifyEditorLayer.hpp"

// TODO: Remove this when I stop using it!
#include <Geode/modify/MenuLayer.hpp>
#include "layers/LobbiesLayer.hpp"


using namespace geode::prelude;

bool MyLevelEditorLayer::init(GJGameLevel* p0, bool p1) {
	
	if (!LevelEditorLayer::init(p0, p1)) {
		return false;
	}

	return true;
}

void MyLevelEditorLayer::onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure) {
	if (pCallback->m_eResult == k_EResultOK) {
		m_fields->m_isInLobby = true;
		m_fields->m_lobbyId = pCallback->m_ulSteamIDLobby;

		// Constants can be changed in CMakeLists.txt
		// Kind of a bad idea but who cares

		// SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "lobby_type", MOD_LOBBY_NAME); // TODO: Uncomment this
		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "version", MOD_VERSION);
	}
}

class $modify(MyEditorPauseLayer, EditorPauseLayer) {

    struct Fields {
		LobbyPopup* m_lobbyPopup = nullptr;
	};

	bool init(LevelEditorLayer* p0) {

		if (!EditorPauseLayer::init(p0)) {
			return false;
		}

		auto hostPopupButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("GJ_hammerIcon_001.png", (1.0F), geode::CircleBaseColor::Cyan, geode::CircleBaseSize::Small),	
			this,
			menu_selector(MyEditorPauseLayer::onHostPopupButton)
		);


		auto menu = this->getChildByID("guidelines-menu");
		menu->addChild(hostPopupButton);
		hostPopupButton->setID("host-button"_spr);
		menu->updateLayout();

		return true;
	}

	void onHostPopupButton(CCObject*) {
		// TODO: add if condition over cast
		auto lvlEditorFields = static_cast<MyLevelEditorLayer*>(m_editorLayer)->m_fields.self();
		if (!lvlEditorFields->m_isInLobby) {
			// TODO: Shorten this
			if (!m_fields->m_lobbyPopup) {
				m_fields->m_lobbyPopup = LobbyPopup::create();
			}
			
			if (m_fields->m_lobbyPopup) {
				m_fields->m_lobbyPopup->show();
			}
		}
		else {
			// TODO: Create diff popup or pass m_isInLobby as an argument
			log::warn("Im too lazy to create a menu ):");
		}
	}

	void onExitEditor(cocos2d::CCObject* sender) {
		leaveLobby(); // Leave Lobby
		EditorPauseLayer::onExitEditor(sender); // And exit editor (whoops)
	}

	void onSaveAndPlay(cocos2d::CCObject* sender) {
		leaveLobby(); // Leave Lobby
		EditorPauseLayer::onSaveAndPlay(sender); // And exit editor (whoops)
	}

	void leaveLobby() {
		// TODO: add if condition over cast
		auto lvlEditorFields = static_cast<MyLevelEditorLayer*>(m_editorLayer)->m_fields.self();

		if (lvlEditorFields->m_isInLobby) {
			SteamMatchmaking()->LeaveLobby(lvlEditorFields->m_lobbyId);
			log::info("Leaving lobby with ID {}", lvlEditorFields->m_lobbyId);
		}
		else {
			log::info("Can't leave lobby because not in lobby!");
		}
    }
};

// TODO: remove this when devtools can compile
class $modify(MenuLayer) {
	void onMoreGames(cocos2d::CCObject* sender) {
		LobbiesLayer::scene();
	}
};