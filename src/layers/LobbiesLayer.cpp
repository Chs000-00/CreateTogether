#include <Geode/Geode.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <isteammatchmaking.h>
#include "../ui/LobbyList.hpp"
#include "../ui/LevelListBorders.hpp"
#include "LobbiesLayer.hpp"

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

    auto menu = CCMenu::create();

    menu->addChildAtPosition(backBtn, Anchor::TopLeft, {25, -25});
    menu->addChildAtPosition(spinner, Anchor::Center);

    this->addChild(menu);

    
    m_fetchFriendsTaskListener.bind([menu](FetchFriendsTask::Event* event) {
        if (event->getValue()){
            auto list = createLobbyList(event->getValue());

            auto listBorders = GDLevelListBorders::create();
            listBorders->setContentSize({356, 220}); // TODO: list->getContentSize()
            menu->addChildAtPosition(list, Anchor::Center, -list->getContentSize() / 2);
            menu->addChildAtPosition(listBorders, Anchor::Center);
        }
    });

    this->refreshLobbyList(refreshBtn); // Bad

    

    return true;
}

// TODO: Fix this
void LobbiesLayer::refreshLobbyList(CCObject* sender) {
    m_fetchFriendsTaskListener.setFilter(loadLobbyList(menu)); // I dont think i'm using tasks right
}


FetchFriendsTask LobbiesLayer::loadLobbyList(CCMenu* menu) {
    return FetchFriendsTask::run([](auto progress, auto hasBeenCancelled) -> std::vector<lobbyData> {

        int friendsCount = SteamFriends()->GetFriendCount( k_EFriendFlagImmediate );
        std::vector<lobbyData> dataVector;
        lobbyData clobby;

        for ( int i = 0; i < friendsCount; i++ )
        {
            FriendGameInfo_t friendGameInfo;
            CSteamID steamIDFriend = SteamFriends()->GetFriendByIndex( i, k_EFriendFlagImmediate );


            // TODO: friendGameInfo.m_steamIDLobby.IsValid() 
            if (SteamFriends()->GetFriendGamePlayed( steamIDFriend, &friendGameInfo) || true) { 
                // I Might not need to do requestLobbyData
                // auto data = SteamMatchmaking()->RequestLobbyData(friendGameInfo.m_steamIDLobby);
                clobby.steamUserName = SteamFriends()->GetFriendPersonaName(steamIDFriend);
                clobby.steamId = friendGameInfo.m_steamIDLobby;

                dataVector.push_back(clobby);   
            } 

        }
        return dataVector;

    }, "Fetches current friends");
}


void LobbiesLayer::keyBackClicked() {
    this->onBack(nullptr);
}
void LobbiesLayer::onBack(CCObject* sender) {
    CCDirector::get()->replaceScene(CCTransitionFade::create(0.5, LevelBrowserLayer::scene(GJSearchObject::create(SearchType::MyLevels))));
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