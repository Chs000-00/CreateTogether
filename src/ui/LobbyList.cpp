#include <Geode/Geode.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <isteammatchmaking.h>
#include "LobbyList.hpp"

using namespace geode::prelude;



ScrollLayer* createLobbyList(std::vector<lobbyData> const& myItems) {
    auto scrollLayer = ScrollLayer::create({ 300, 200 });
  
  	scrollLayer->m_contentLayer->setLayout(
    	ColumnLayout::create()
      		->setGap(0)
      		->setAxisReverse(true)
      		->setAxisAlignment(AxisAlignment::End)
      		->setAutoGrowAxis(scrollLayer->getContentHeight())
    );
    

    bool alternateColorBG = false;

    for (auto const& myItem : myItems) {
        auto node = CCNode::create();

        node->setContentSize({ scrollLayer->getContentWidth(), 50 });

        // If you want to create a solid rectangular BG, use CCLayerColor
        // If you want to create a rounded rectangle like the new Geode UI, 
        // use CCScale9Sprite with square02b_001.png as the sprite
        auto bg = CCLayerColor::create(
            // Alternating darker and lighter color BGs
            ccc4(0, 0, 0, alternateColorBG ? 90 : 50),
            node->getContentWidth(), node->getContentHeight()
        );

        bg->ignoreAnchorPointForPosition(false);
        // Always prefer addChildAtPosition over addChild if possible
        node->addChildAtPosition(bg, Anchor::Center);

        // Add Buttons and whatnot here using addChildAtPosition

      	// IMPORTANT: Always add your list nodes as children to the 
      	// *content layer* - never the list directly!
		// We don't need to handle positioning here as the content layer is 
      	// managed by the ColumnLayout
      	scrollLayer->m_contentLayer->addChild(node);
      
        alternateColorBG = !alternateColorBG;
    }
  
  	scrollLayer->m_contentLayer->updateLayout();
  	return scrollLayer;
}

