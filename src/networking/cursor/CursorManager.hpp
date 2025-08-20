#pragma once

#include <ctserialize_generated.h>
#include <ctcursor_generated.h>
#include <string>
#include "../../types/Cursor.hpp"

#ifdef STEAMWORKS
    #include <isteamnetworkingmessages.h>
    #include <isteamuser.h>
    #include <isteammatchmaking.h>
#else
    #include <debug/isteamnetworkingmessages.h>
#endif

using namespace geode::prelude;

class CursorManager {
    public:

        static CursorManager* get();
        static CreateTogetherCursor* getPlayerCursor(SteamNetworkingIdentity id);

        void sendCursorUpdateToAll();
        void updateCursorPositon(CreateTogetherCursor* cursor, CCPoint position);
        void update();
        void cursorNetworkingPrelude();
        void receiveCursorData();
        Result<uint8_t> parseCursorData(const CTSerialize::cursor::CursorUpdate* msg, SteamNetworkingIdentity msgSource);
        
        std::unordered_map<std::string, CreateTogetherCursor*> m_playerCursors;

        #ifdef NO_STEAMWORKS
            HSteamNetConnection m_cursorConnection;
        #endif

        flatbuffers::FlatBufferBuilder m_cursorBuilder;

    private:
        void sendMessage(flatbuffers::Offset<CTSerialize::cursor::CursorUpdate> out);
};