#include <Geode/Geode.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <isteammatchmaking.h>
#include <isteamfriends.h>
#include "../layers/LobbiesLayer.hpp"
#include "../types/LobbyData.hpp"

using namespace geode::prelude;

ScrollLayer* LobbiesLayer::createLobbyList(std::vector<lobbyData> lobbyList) {
    auto scrollLayer = ScrollLayer::create({ 356, 220 });
  
  	scrollLayer->m_contentLayer->setLayout(
    	ColumnLayout::create()
      		->setGap(0)
      		->setAxisReverse(true)
      		->setAxisAlignment(AxisAlignment::End)
      		->setAutoGrowAxis(scrollLayer->getContentHeight())
    );
    

    bool alternateColorBG = false;
  
    for (auto lobby = lobbyList.begin(); lobby != lobbyList.end(); ++lobby) {
        auto node = CCNode::create();
        auto menu = CCMenu::create();


        menu->setContentSize({ scrollLayer->getContentWidth(), 30 });
        node->setContentSize({ scrollLayer->getContentWidth(), 30 });


        auto color = ccc4(161, 88, 44, 255);
        if (alternateColorBG) {
          color = ccc4(194, 114, 62, 255);
        }

        auto bg = CCLayerColor::create(
            // Alternating darker and lighter color BGs
            color,
            node->getContentWidth(), node->getContentHeight()
        );

        // log::debug("Creating listnode for user: {}", lobby->steamUserName);

        bg->ignoreAnchorPointForPosition(false);
        // Always prefer addChildAtPosition over addChild if possible
        node->addChildAtPosition(bg, Anchor::Center);

        // Add Buttons and whatnot here using addChildAtPosition
        // IMPORTANT: Always add your list nodes as children to the 
      	// *content layer* - never the list directly!
        // We don't need to handle positioning here as the content layer is 
      	// managed by the ColumnLayout

        if (lobby->isVersionMismatched) {
          auto mismatchedVersionLabel = CCLabelBMFont::create("Mismatched version detected!", "bigFont.fnt");
          mismatchedVersionLabel->setColor(ccc3(255, 49, 49));
          mismatchedVersionLabel->setAnchorPoint({0, mismatchedVersionLabel->getScaleX()/2});
          node->addChildAtPosition(mismatchedVersionLabel, Anchor::Left, {20, 0});
          scrollLayer->m_contentLayer->addChild(node);
          node->setZOrder(9);
          node->addChildAtPosition(menu, Anchor::Center);
        
          alternateColorBG = !alternateColorBG;
          continue;
        }
        
        auto steamUsernameLabel = CCLabelBMFont::create(lobby->steamUserName.c_str(), "bigFont.fnt");
        steamUsernameLabel->setAnchorPoint({0, steamUsernameLabel->getScaleX()/2});
        steamUsernameLabel->setScale(0.5f);
        node->addChildAtPosition(steamUsernameLabel, Anchor::Left, {20, 0});

        auto joinBtn = CCMenuItemSpriteExtra::create(
          ButtonSprite::create("Join"),
          scrollLayer,
          menu_selector(LobbiesLayer::onJoin)
        );

        joinBtn->setScale(0.8f);
        joinBtn->setTag(std::distance(lobbyList.begin(), lobby));
        menu->addChildAtPosition(joinBtn, Anchor::Right, {-40, 0});

      	
      	scrollLayer->m_contentLayer->addChild(node);
        node->setZOrder(9);
        node->addChildAtPosition(menu, Anchor::Center);
      
        alternateColorBG = !alternateColorBG;
    }
  
  	scrollLayer->m_contentLayer->updateLayout();
    scrollLayer->setTouchEnabled(true);

  	return scrollLayer;
}

