#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <isteammatchmaking.h>

using namespace geode::prelude;

struct lobbyData {
    CSteamID steamId;
    std::string steamUserName;
    std::string levelName;
};

std::vector<lobbyData> fetchFriends();

ScrollLayer* createLobbyList(std::vector<lobbyData>* lobbyList);