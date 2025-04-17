#pragma once

#define MOD_LOBBY_NAME "CreateTogether"
#define MOD_VERSION "v0.3.1"

#define DEDICATED_PORT 24018

#ifndef USE_TEST_SERVER
    #define NETWORKING_MSG SteamNetworkingMessage_t
#else
    #define NETWORKING_MSG TestServerMsg
#endif

namespace constants {
    const int LARGEST_SPEED = 5; // Right now this is Speed::Fastest
    const int LARGEST_SELECT_ART_TYPE = 2; // SelectArtType::Ground
}