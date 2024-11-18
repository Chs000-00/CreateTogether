#include <Geode/Geode.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <isteammatchmaking.h>
#include <isteamfriends.h>
#include "LobbyList.hpp"

using namespace geode::prelude;


// TODO: Clean this code up
std::vector<lobbyData> fetchFriends() {
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
}


ScrollLayer* createLobbyList(std::vector<lobbyData> const& lobbyList) {
    auto scrollLayer = ScrollLayer::create({ 356, 220 });

    
  
  	scrollLayer->m_contentLayer->setLayout(
    	ColumnLayout::create()
      		->setGap(0)
      		->setAxisReverse(true)
      		->setAxisAlignment(AxisAlignment::End)
      		->setAutoGrowAxis(scrollLayer->getContentHeight())
    );
    

    bool alternateColorBG = false;


    for (auto const& lobby : lobbyList) {
        auto node = CCNode::create();

        node->setContentSize({ scrollLayer->getContentWidth(), 50 });


        auto color = ccc4(161, 88, 44, 255);
        if (alternateColorBG) {
          color = ccc4(194, 114, 62, 255);
        }

        auto bg = CCLayerColor::create(
            // Alternating darker and lighter color BGs
            color,
            node->getContentWidth(), node->getContentHeight()
        );

        // log::debug("Creating listnode for user: {}", lobby.steamUserName);

        bg->ignoreAnchorPointForPosition(false);
        // Always prefer addChildAtPosition over addChild if possible
        node->addChildAtPosition(bg, Anchor::Center);

        // Add Buttons and whatnot here using addChildAtPosition
        auto steamUsernameLabel = CCLabelBMFont::create(lobby.steamUserName.c_str(), "bigFont.fnt");
        steamUsernameLabel->setScale(0.5);
        node->addChildAtPosition(steamUsernameLabel, Anchor::Center);

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

