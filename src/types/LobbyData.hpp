#pragma once

#include "../include.hpp"


using namespace geode::prelude;

struct lobbyData {
    CSteamID steamId;
    std::string steamUserName;
    std::string levelName;
    bool isVersionMismatched;
};
