#pragma once

#include <isteamnetworkingmessages.h>

// Because it is so easy to test steam p2p applications, here is a "fake" msg class
// To be used with the test server
struct TestServerMsg {
    char m_data[1024];
    SteamNetworkingIdentity m_identityPeer; // DO NOT ACCESS!!!!
    void Release() {};

    const char* GetData() {
        return this->m_data;
    };
};