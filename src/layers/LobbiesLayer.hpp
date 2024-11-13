#pragma once

#include <Geode/Geode.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>

using namespace geode::prelude;

class LobbiesLayer : public cocos2d::CCLayer {
protected:
    bool init() override;
    void onLobbyListRetrieved( LobbyMatchList_t *pLobbyMatchList, bool bIOFailure );

public:
    static LobbiesLayer* scene();
    static LobbiesLayer* create();
    void onBack(CCObject* sender);
};