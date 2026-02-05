#ifdef STEAMWORKS

#include <Geode/Geode.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/binding/TableView.hpp>

// #ifdef STEAMWORKS
    #include <isteammatchmaking.h>
// s#endif

#include "LobbiesLayer.hpp"
#include "../ui/LevelListBorder.hpp"
#include "../config.hpp"
#include "SteamManager.hpp"

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

    // TODO: Add this as a member when I get intellisense access
    auto spinner = LoadingSpinner::create(100);
    spinner->setID("loading-spinner");

    addSideArt(this, SideArt::All, SideArtStyle::Layer);
    
    backBtn->setID("back-btn");


    auto refreshBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
        this,
        menu_selector(LobbiesLayer::refreshLobbyList)
    );

    auto listBorders = LevelListBorders::create();
    listBorders->setPadding(20.0f);
    listBorders->setContentSize({356, 270}); // TODO: list->getContentSize()
    

    this->m_menu = CCMenu::create();

    m_menu->retain();
    
    m_menu->addChildAtPosition(listBorders, Anchor::Center);
    m_menu->addChildAtPosition(backBtn, Anchor::TopLeft, {25, -25});
    m_menu->addChildAtPosition(spinner, Anchor::Center);
    m_menu->addChildAtPosition(refreshBtn, Anchor::BottomRight, {-25, 25});
    this->addChild(m_menu);

    this->refreshLobbyList(nullptr);

    return true;
}


void LobbiesLayer::refreshLobbyList(CCObject* sender) {
    log::info("Refreshing lobby list");

    this->m_menu->getChildByID("loading-spinner")->setVisible(true);

    // SteamMatchmaking()->AddRequestLobbyListDistanceFilter(k_ELobbyDistanceFilterFar);
    SteamMatchmaking()->AddRequestLobbyListStringFilter("lobby_type", MOD_LOBBY_ID, k_ELobbyComparisonEqual);
	SteamAPICall_t hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
    this->m_lobbyMatchListCallResult.Set(hSteamAPICall, this, &LobbiesLayer::onLobbyMatchList);
}


void LobbiesLayer::keyBackClicked() {
    this->onBack(nullptr);
}

void LobbiesLayer::onBack(CCObject* sender) {
    CC_SAFE_RELEASE(this->m_menu);
    CCDirector::get()->replaceScene(CCTransitionFade::create(0.5, LevelBrowserLayer::scene(GJSearchObject::create(SearchType::MyLevels))));
}

void LobbiesLayer::onJoin(CCObject* sender) {
    auto lobbyID = this->m_data.at(sender->getTag()).steamId;
    log::info("Joining lobby with steamID: {}", lobbyID.ConvertToUint64());
	SteamManager::get()->m_lobbyJoined = SteamMatchmaking()->JoinLobby(lobbyID); 
}

void LobbiesLayer::loadDataToList() {
    auto menu = this->m_menu;

    auto scrollLayer = menu->getChildByID("scroll-layer");
    auto listBorders = menu->getChildByID("list-borders");

    if (scrollLayer) {
        scrollLayer->removeFromParent();
    }

    scrollLayer = LobbiesLayer::createLobbyList(this->m_data);
    scrollLayer->setID("scroll-layer");

    menu->addChildAtPosition(scrollLayer, Anchor::Center, -scrollLayer->getContentSize() / 2);

    if (this->m_data.size() != 0) {
        listBorders->setVisible(true);
    }
    else {
        log::info("No lobbies found!");
        FLAlertLayer::create(
            "No lobbies",
            "No one is hosting a lobby!",
            "Ok"
        )->show();
    }
    menu->getChildByID("loading-spinner")->setVisible(false);
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

void LobbiesLayer::onLobbyMatchList(LobbyMatchList_t *pLobbyMatchList, bool bIOFailure) {	
    std::vector<lobbyData> dataVector;
    lobbyData clobby;

    for (int i = 0; i < pLobbyMatchList->m_nLobbiesMatching; i++) {
        CSteamID lobbyID = SteamMatchmaking()->GetLobbyByIndex(i);
        
        SteamMatchmaking()->RequestLobbyData(lobbyID);

        log::info("Lobby #{}", i);

        if (SteamMatchmaking()->GetLobbyData(lobbyID, "version") != MOD_VERSION) {
            clobby.isVersionMismatched = true;
            dataVector.push_back(clobby);  
            continue;
        }

        clobby.isVersionMismatched = false;


        clobby.levelName = SteamMatchmaking()->GetLobbyData(lobbyID, "level_name");
        clobby.steamUserName = SteamMatchmaking()->GetLobbyData(lobbyID, "host_name");

        clobby.steamId = lobbyID;

        dataVector.push_back(clobby);   
        log::debug("SteamID: {} | LobbyID: {}", clobby.steamId.ConvertToUint64(), lobbyID.ConvertToUint64());

        this->m_data = dataVector;
    }

    this->loadDataToList();
}

#endif