#pragma once

#ifdef STEAMWORKS

#include <Geode/Geode.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>

//#ifdef STEAMWORKS
    #include <isteammatchmaking.h>
//#endif

#include "../types/LobbyData.hpp"


using namespace geode::prelude;


class LobbiesLayer : public cocos2d::CCLayer {
protected:

    // This causes crash ):
    // TODO: Fix
    CCMenu* m_menu;

    bool init() override;
    void keyBackClicked() override;
    void onLobbyListRetrieved( LobbyMatchList_t *pLobbyMatchList, bool bIOFailure );

public:
    std::vector<lobbyData> m_data;


	CCallResult< LobbiesLayer, LobbyMatchList_t  > m_lobbyMatchListCallResult;
	void onLobbyMatchList(LobbyMatchList_t *pLobbyMatchList, bool bIOFailure);


    static LobbiesLayer* scene();
    static LobbiesLayer* create();
    static ScrollLayer* createLobbyList(std::vector<lobbyData> lobbyList);
    void refreshLobbyList(CCObject* sender);
    void onBack(CCObject* sender);
    void onJoin(CCObject* sender);
    void loadDataToList();
};

#endif
