#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <isteammatchmaking.h>
#include "../layers/LobbiesLayer.hpp"

#ifdef USE_TEST_SERVER
    #include "ModifyGameManager.hpp"
	#include <WinSock2.h>
#endif


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
        #ifdef USE_TEST_SERVER

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
        #ifdef USE_TEST_SERVER
            joinServer(false);
        #else
            LobbiesLayer::scene();
        #endif
	}
    #ifdef USE_TEST_SERVER
	void onJoinServerAsHost(cocos2d::CCObject* sender) {
        // If the test server is enabled, connect to it instead.
        joinServer(true);
	}


    void joinServer(bool asHost) {
        auto gameManager = static_cast<MyGameManager*>(GameManager::get());
        gameManager->m_fields->m_socket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(DEDICATED_PORT);
        serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
        connect(gameManager->m_fields->m_socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

        u_long mode = 1;  // 1 to enable non-blocking socket
        ioctlsocket(gameManager->m_fields->m_socket, FIONBIO, &mode);


        gameManager->m_fields->m_isInLobby = true;
        gameManager->m_fields->m_isHost = asHost;
        gameManager->enterLevelEditor();
    }
    #endif
    
};
