#pragma once

#include <Geode/Geode.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <isteammatchmaking.h>
#include "../LobbyData.hpp"


using namespace geode::prelude;
using FetchFriendsTask = Task<std::vector<lobbyData>>;

class LobbiesLayer : public cocos2d::CCLayer {
protected:

    std::vector<lobbyData>* m_data;

    bool init() override;
    void keyBackClicked() override;
    void onLobbyListRetrieved( LobbyMatchList_t *pLobbyMatchList, bool bIOFailure );
    EventListener<FetchFriendsTask> m_fetchFriendsTaskListener;

public:
    static LobbiesLayer* scene();
    static LobbiesLayer* create();
    static ScrollLayer* createLobbyList(std::vector<lobbyData>* lobbyList);
    FetchFriendsTask loadLobbyList();
    void refreshLobbyList(CCObject* sender);
    void onBack(CCObject* sender);
    void onJoin(CCObject* sender);
};