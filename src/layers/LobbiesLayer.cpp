#include <Geode/Geode.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <isteammatchmaking.h>
#include "LobbiesLayer.hpp"
#include "../hooks/ModifyGameManager.hpp"
#include "../LobbyData.hpp"
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

    auto menu = CCMenu::create();

    menu->addChildAtPosition(backBtn, Anchor::TopLeft, {25, -25});
    menu->addChildAtPosition(spinner, Anchor::Center);

    // Couldn't some sort of race condition happen here if the user somehow presses the button before the bind happens?
    menu->addChildAtPosition(refreshBtn, Anchor::BottomRight);


    this->addChild(menu);

    // Honestly I dont think im using tasks right here
    // TODO: Use Steam Callbacks instead
    m_fetchFriendsTaskListener.bind([menu, this](FetchFriendsTask::Event* event) {
        this->m_data = event->getValue();
        auto list = LobbiesLayer::createLobbyList(event->getValue());
        auto listBorders = GDLevelListBorders::create();
        listBorders->setContentSize({356, 220}); // TODO: list->getContentSize()
        menu->addChildAtPosition(list, Anchor::Center, -list->getContentSize() / 2);
        menu->addChildAtPosition(listBorders, Anchor::Center);
    });

    this->refreshLobbyList(nullptr);

    

    return true;
}

void LobbiesLayer::refreshLobbyList(CCObject* sender) {
    // TODO: Fetch lobbies
    m_fetchFriendsTaskListener.setFilter(loadLobbyList());
    log::error("Feature not implemented yet! Might break!");
}

// TODO: Replace function with one that fetches current lobbies instead of friends!
FetchFriendsTask LobbiesLayer::loadLobbyList() {
    return FetchFriendsTask::run([](auto progress, auto hasBeenCancelled) -> std::vector<lobbyData> {

        int friendsCount = SteamFriends()->GetFriendCount( k_EFriendFlagImmediate );
        std::vector<lobbyData> dataVector;
        lobbyData clobby;

        for (int i = 0; i < friendsCount; i++)
        {
            FriendGameInfo_t friendGameInfo;
            CSteamID steamIDFriend = SteamFriends()->GetFriendByIndex( i, k_EFriendFlagImmediate );


            if (SteamFriends()->GetFriendGamePlayed( steamIDFriend, &friendGameInfo) || friendGameInfo.m_steamIDLobby.IsValid() ) { 
                // auto data = SteamMatchmaking()->RequestLobbyData(friendGameInfo.m_steamIDLobby);
                clobby.steamUserName = SteamFriends()->GetFriendPersonaName(steamIDFriend);
                clobby.steamId = friendGameInfo.m_steamIDLobby;

                dataVector.push_back(clobby);   
                log::info("Data stuff: {} | {}", clobby.steamId.ConvertToUint64(), steamIDFriend.ConvertToUint64());
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

void LobbiesLayer::onJoin(CCObject* sender) {
    auto lobbyID = this->m_data->at(sender->getTag()).steamId;
    log::info("Joining lobby with steamID: {}", lobbyID.ConvertToUint64());
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
	gameManager->m_fields->m_lobbyJoined = SteamMatchmaking()->JoinLobby(lobbyID); 
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