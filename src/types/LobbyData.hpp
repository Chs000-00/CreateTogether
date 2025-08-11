#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/binding/SimplePlayer.hpp>

#ifdef STEAMWORKS
    #include <isteammatchmaking.h>
#endif

using namespace geode::prelude;

#ifdef STEAMWORKS

struct lobbyData {
    CSteamID steamId;
    std::string steamUserName;
    std::string levelName;
    bool isVersionMismatched;
};

#endif


struct lobbyOptions {
    bool isPrivate = true;
    // TODO: Implement
    bool canFriendsJoin = true;
};

