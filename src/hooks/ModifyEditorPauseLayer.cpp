#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include "ModifyGameManager.hpp"
#include "ModifyEditorLayer.hpp"
#include "../ui/LobbyPopup.hpp"
#include "../networking/NetManager.hpp"


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

	void onHostPopupButton(CCObject* sender) {
		// TODO: add if condition over cast
		auto netManager = NetManager::get();

		if (!NetManager::getIsInLobby()) {
			// TODO: Shorten this
			m_fields->m_lobbyPopup = LobbyPopup::create(eLobbyHostPopup);
			
		} 
		else if (netManager->m_isHost) {
			m_fields->m_lobbyPopup = LobbyPopup::create(eLobbyHostingPopup);
		} 
		else {
			m_fields->m_lobbyPopup = LobbyPopup::create(eLobbyJoinedUserPopup);
		}


		// Make sure we really did in fact create a popup
		if (m_fields->m_lobbyPopup) {
			m_fields->m_lobbyPopup->show();
		}

	}

	// TODO: There might be an issue with exiting the lobby before you get to confirm exiting on the popup.

	void onExitEditor(CCObject* sender) {
		auto gameManager = static_cast<MyGameManager*>(GameManager::get());
		static_cast<MyLevelEditorLayer*>(this->m_editorLayer)->m_fields->m_pUniqueIDOfGameObject->release();
		NetManager::get()->leaveLobby();
		EditorPauseLayer::onExitEditor(sender); // And exit editor (whoops)
	}

	void onExitNoSave(CCObject* sender) {
		auto gameManager = static_cast<MyGameManager*>(GameManager::get());
		static_cast<MyLevelEditorLayer*>(this->m_editorLayer)->m_fields->m_pUniqueIDOfGameObject->release();
		NetManager::get()->leaveLobby();
		EditorPauseLayer::onExitNoSave(sender);
	}

	void onSaveAndPlay(CCObject* sender) {
		auto netManager = NetManager::get();
		// gameManager->leaveLobby(); // Leave Lobby

		if (!NetManager::getIsInLobby()) {
			EditorPauseLayer::onSaveAndPlay(sender); // And exit editor (whoops)
		}
		else {
			// TODO: Playlayer stuff
		}
	}

};

