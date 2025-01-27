#pragma once

#include <Geode/Geode.hpp>
#include <isteamnetworkingmessages.h>

using namespace geode::prelude;

#ifndef USE_TEST_SERVER
    #pragma message ("Placebo message is included yet test server is not defined!")
#endif

// Because it is so easy to test steam p2p applications, here is a "fake" msg class
// To be used with the test server
struct TestServerMsg {
    char m_data[1024];
    SteamNetworkingIdentity m_identityPeer; // DO NOT ACCESS!!!!
    inline void Release() {
        // log::debug("Destroying fake message!");
        delete this;
    };

    size_t GetSize() {
        return strlen(m_data);
    }

    const char* GetData() {
        return this->m_data;
    };   
}; 