#include <Geode/Geode.hpp>
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


    // TODO: Put this in a task
    auto vfriends = fetchFriends();
    auto list = createLobbyList(vfriends);

    auto listBorders = GDLevelListBorders::create();
    listBorders->setContentSize({356, 220});
    

    auto menu = CCMenu::create();
    menu->addChildAtPosition(backBtn, Anchor::TopLeft, {25, -25});
    menu->addChildAtPosition(spinner, Anchor::Center);
    menu->addChildAtPosition(list, Anchor::Center, -list->getContentSize() / 2);
    menu->addChildAtPosition(listBorders, Anchor::Center);



    this->addChild(menu);


    return true;

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