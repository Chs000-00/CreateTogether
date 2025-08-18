#pragma once

#include <ctserialize_generated.h>
#include <ctcursor_generated.h>
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
        static std::vector<CreateTogetherCursor*> getPlayerCursors();

        void sendCursorUpdateToAll();
        void updateCursorPositon(CreateTogetherCursor* cursor, CCPoint position);
        void update();
        void cursorNetworkingPrelude();
        void receiveCursorData();
        Result<uint8_t> parseCursorData(const CTSerialize::MessageHeader* msg, SteamNetworkingIdentity msgSource);
        
        std::vector<CreateTogetherCursor*> m_playerCursors;


        #ifdef NO_STEAMWORKS
            HSteamNetConnection m_cursorConnection;
        #endif

        flatbuffers::FlatBufferBuilder m_cursorBuilder;

    private:
        void sendMessage(flatbuffers::Offset<CTSerialize::cursor::CursorUpdate> out);
};