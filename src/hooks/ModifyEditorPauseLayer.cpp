#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <isteammatchmaking.h>
#include "ModifyGameManager.hpp"
#include "../ui/LobbyPopup.hpp"

using namespace geode::prelude;


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
		auto gameManager = GameManager::get();
		auto gameManagerFields = static_cast<MyGameManager*>(gameManager)->m_fields.self();

		if (!gameManagerFields->m_isInLobby) {
			// TODO: Shorten this
			if (!m_fields->m_lobbyPopup) {
				m_fields->m_lobbyPopup = LobbyPopup::create();
			}
			
			// Make sure we really did in fact create a popup
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
		// leaveLobby(); // Leave Lobby
		EditorPauseLayer::onExitEditor(sender); // And exit editor (whoops)
	}

	void onSaveAndPlay(cocos2d::CCObject* sender) {
		leaveLobby(); // Leave Lobby
		EditorPauseLayer::onSaveAndPlay(sender); // And exit editor (whoops)
	}


	// Debug Button
	void onAlignX(cocos2d::CCObject* sender) {
		auto gameManager = static_cast<MyGameManager*>(GameManager::get());
		gameManager->fetchMemberList();
		log::debug("LobbyID: {} | IsInEditorLayer: {} | IsHost: {} | IsInLobby: {}", gameManager->m_fields->m_lobbyId, gameManager->m_fields->m_isInEditorLayer, gameManager->m_fields->m_isHost, gameManager->m_fields->m_isInLobby);
		
		// This should output the same values
		log::debug("LevelEditorLayer::get(): {} | gameManager->m_fields->m_level: {}", fmt::ptr(LevelEditorLayer::get()), fmt::ptr(gameManager->m_fields->m_level));
	}



	void leaveLobby() {
		// TODO: add if condition over cast
		auto gameManager = GameManager::get();
		auto gameManagerFields = static_cast<MyGameManager*>(gameManager)->m_fields.self();

		gameManagerFields->m_isInEditorLayer = false;

		if (gameManagerFields->m_isInLobby) {
			log::info("Leaving lobby with ID {}", gameManagerFields->m_lobbyId);
			SteamMatchmaking()->LeaveLobby(gameManagerFields->m_lobbyId);
			gameManagerFields->m_lobbyCreated = 0;
			gameManagerFields->m_lobbyJoined = 0;
			gameManagerFields->m_lobbyId = 0;
			gameManagerFields->m_isInLobby = false;
		}
		else {
			log::info("Can't leave lobby because not in lobby!");
		}
    }
};

