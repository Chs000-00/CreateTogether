#include <Geode/Geode.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <isteammatchmaking.h>
#include "LobbiesLayer.hpp"
#include "../ui/LobbyList.hpp"

using namespace geode::prelude;

bool LobbiesLayer::init() {
    if (!CCLayer::init())
        return false;

    GameManager::get()->fadeInMenuMusic();

    this->setID("LobbiesLayer");

    auto background = createLayerBG();
    background->setID("background");
    this->addChild(background);

    addSideArt(this, geode::SideArt::All, geode::SideArtStyle::Layer);

    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(LobbiesLayer::onBack)
    );

    backBtn->setID("back-btn");
    auto menu = CCMenu::create();
    menu->addChildAtPosition(backBtn, geode::Anchor::TopLeft, {25, -25});




    this->addChild(menu);

    return true;

}

// void LobbiesLayer::onLobbyListRetrieved( LobbyMatchList_t *pLobbyMatchList, bool bIOFailure ) {
//     // TODO: FINISH THIS FUNC
//     auto list = createLobbyList();
//     menu->addChildAtPosition(list, Anchor::Center, -list->getContentSize() / 2);
// }

void LobbiesLayer::keyBackClicked() {
    this->onBack(nullptr);
}
void LobbiesLayer::onBack(CCObject* sender) {
    CCDirector::get()->replaceScene(CCTransitionFade::create(0.5, CreatorLayer::scene()));
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