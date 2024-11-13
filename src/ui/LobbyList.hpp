#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/ScrollLayer.hpp>

using namespace geode::prelude;

struct lobbyData {
    std::string gdUserName;
    std::string steamUserName;
    std::string levelName;
};

void requestAndCreateLobby(CCMenu* pMenu);

ScrollLayer* createLobbyList(std::vector<lobbyData> const& myItems);