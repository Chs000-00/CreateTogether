#include <Geode/Geode.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <isteammatchmaking.h>
#include "LobbiesLayer.hpp"
#include "../hooks/ModifyGameManager.hpp"
#include "../ui/LevelListBorders.hpp"

using namespace geode::prelude;


bool LobbiesLayer::init() {
    if (!CCLayer::init()) {
        return false;
    }

    GameManager::get()->fadeInMenuMusic();

    this->setID("LobbiesLayer");
    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);


    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(LobbiesLayer::onBack)
    );

    auto background = createLayerBG();
    background->setID("background");
    this->addChild(background);

    auto spinner = LoadingSpinner::create(100);

    addSideArt(this, SideArt::All, SideArtStyle::Layer);
    
    backBtn->setID("back-btn");


    auto refreshBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
        this,
        menu_selector(LobbiesLayer::refreshLobbyList)
    );

    auto m_menu = CCMenu::create();

    m_menu->addChildAtPosition(backBtn, Anchor::TopLeft, {25, -25});
    m_menu->addChildAtPosition(spinner, Anchor::Center);
    m_menu->addChildAtPosition(refreshBtn, Anchor::BottomRight, {-25, 25});
    this->addChild(m_menu);

    this->refreshLobbyList(nullptr);

    return true;
}


void LobbiesLayer::refreshLobbyList(CCObject* sender) {
    log::info("Refreshing lobby list");
	SteamAPICall_t hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
    auto gameManagerCast = static_cast<MyGameManager*>(GameManager::get());
    auto gameManagerFields = gameManagerCast->m_fields.self();
    gameManagerFields->m_lobbyLayer = this;
    // SteamMatchmaking()->AddRequestLobbyListDistanceFilter(k_ELobbyDistanceFilterFar);
    gameManagerFields->m_lobbyMatchListCallResult.Set(hSteamAPICall, gameManagerCast, &MyGameManager::onLobbyMatchList);
}


void LobbiesLayer::keyBackClicked() {
    this->onBack(nullptr);
}
void LobbiesLayer::onBack(CCObject* sender) {
    CCDirector::get()->replaceScene(CCTransitionFade::create(0.5, LevelBrowserLayer::scene(GJSearchObject::create(SearchType::MyLevels))));
}

void LobbiesLayer::onJoin(CCObject* sender) {
    auto lobbyID = this->m_data->at(sender->getTag()).steamId;
    log::info("Joining lobby with steamID: {}", lobbyID.ConvertToUint64());
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
	gameManager->m_fields->m_lobbyJoined = SteamMatchmaking()->JoinLobby(lobbyID); 
}

void LobbiesLayer::loadDataToList() {
    if (!this->m_data) {
        log::info("No lobbies found!");
        return;
    }

    if (this->m_scrollLayer) {
        this->m_scrollLayer->removeFromParent();
    }

    this->m_scrollLayer = LobbiesLayer::createLobbyList(this->m_data);
    m_scrollLayer->setID("scroll-layer");

    m_menu->addChildAtPosition(this->m_scrollLayer, Anchor::Center, -this->m_scrollLayer->getContentSize() / 2);

    if (!this->m_listBorders) {
        this->m_listBorders = GDLevelListBorders::create();
        this->m_listBorders->setContentSize({356, 270}); // TODO: list->getContentSize()
        this->m_listBorders->setZOrder(5);
        m_menu->addChildAtPosition(this->m_listBorders, Anchor::Center);
    }

}


void LobbiesLayer::fetchLobbies(unsigned int amountOfLobbiesFound) {
	lobbyData clobby;

	for (int i = 0; i < amountOfLobbiesFound; i++) {
		CSteamID lobbyID = SteamMatchmaking()->GetLobbyByIndex(i);

		log::info("lobbyf");

		if (SteamMatchmaking()->GetLobbyData(lobbyID, "version") != MOD_VERSION) {
			clobby.isVersionMismatched = true;
			this->m_data->push_back(clobby);  
			continue;
		}

		clobby.isVersionMismatched = false;

		SteamMatchmaking()->RequestLobbyData(lobbyID);

		clobby.levelName = SteamMatchmaking()->GetLobbyData(lobbyID, "level_name");
		clobby.steamUserName = SteamMatchmaking()->GetLobbyData(lobbyID, "host_name");

		clobby.steamId = lobbyID;

		this->m_data->push_back(clobby);   
		log::debug("Data stuff: {} | {}", clobby.steamId.ConvertToUint64(), lobbyID.ConvertToUint64());
	}
}

LobbiesLayer* LobbiesLayer::create() {
    auto ret = new LobbiesLayer;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}


LobbiesLayer* LobbiesLayer::scene() {
    auto layer = LobbiesLayer::create();
    switchToScene(layer);
    return layer;
}