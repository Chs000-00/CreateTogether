#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

#ifdef STEAMWORKS
    #include <isteammatchmaking.h>
#endif

#include "../layers/LobbiesLayer.hpp"
#include "../networking/NetManager.hpp"



using namespace geode::prelude;

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

        // Horrible
        #ifdef NO_STEAMWORKS

            auto joinAsHostBtn = CCMenuItemSpriteExtra::create(
                // "GJ_hammerIcon_001.png" Possibly?
                CircleButtonSprite::createWithSpriteFrameName("ring_01_001.png", (1.0F), geode::CircleBaseColor::Cyan),	
                this,
                menu_selector(MyLevelBrowserLayer::onJoinServerAsHost)
            );

            menu->addChild(joinAsHostBtn);

        #endif


        menu->addChild(openSearchForLobbyLayer);
        openSearchForLobbyLayer->setID("open-lobby-search-button"_spr);
        menu->updateLayout();

        return true;
	}		

	void onLobbySearchButton(cocos2d::CCObject* sender) {

        // If the test server is enabled, connect to it instead.
        #ifdef NO_STEAMWORKS
            joinServer(false);
        #else
            LobbiesLayer::scene();
        #endif
	}
    
    #ifdef NO_STEAMWORKS

	void onJoinServerAsHost(cocos2d::CCObject* sender) {
        // If the test server is enabled, connect to it instead.
        joinServer(true);
	}


    void joinServer(bool asHost) {

    }
    
    #endif
    
};
