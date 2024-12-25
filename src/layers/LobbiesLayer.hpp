#pragma once
#include "../include.hpp"



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


    static LobbiesLayer* scene();
    static LobbiesLayer* create();
    static ScrollLayer* createLobbyList(std::vector<lobbyData> lobbyList);
    void refreshLobbyList(CCObject* sender);
    void onBack(CCObject* sender);
    void onJoin(CCObject* sender);
    void loadDataToList();
    void fetchLobbies(unsigned int amountOfLobbiesFound);
};