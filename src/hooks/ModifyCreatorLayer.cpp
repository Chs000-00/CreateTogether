#include "../include.hpp"


class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {

    bool init(GJSearchObject* p0) {
		if (!LevelBrowserLayer::init(p0)) {
			return false;
		}

        if (p0->m_searchType != SearchType::MyLevels) {
            return true;
        }


        auto openSearchForLobbyLayer = CCMenuItemSpriteExtra::create(
            // "GJ_hammerIcon_001.png" Possibly?
            CircleButtonSprite::createWithSpriteFrameName("GJ_sFriendsIcon_001.png", (1.0F), geode::CircleBaseColor::Green),	
            this,
            menu_selector(MyLevelBrowserLayer::onLobbySearchButton)
        );


        auto menu = this->getChildByID("my-levels-menu");
        menu->addChild(openSearchForLobbyLayer);
        openSearchForLobbyLayer->setID("open-lobby-search-button"_spr);
        menu->updateLayout();

        return true;
	}		

	void onLobbySearchButton(cocos2d::CCObject* sender) {

        // If the test server is enabled, connect to it instead.
        #ifdef USE_TEST_SERVER
            auto gameManager = static_cast<MyGameManager*>(GameManager::get());
            gameManager->m_fields->m_socket = INVALID_SOCKET;

            gameManager->enterLevelEditor();
        
        #else
            LobbiesLayer::scene();
        #endif
	}
};