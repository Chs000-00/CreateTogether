#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <isteamfriends.h>
#include <steam_api_common.h>

// Fixed version of STEAM_CALLBACK
#define STEAM_CALLBACK_FIXED(thisclass, func, param) \
    void func(param * pParam); \
	struct CCallbackInternal_ ## func : private CCallbackImpl< sizeof( param ) > { \
		CCallbackInternal_ ## func () { SteamAPI_RegisterCallback( this, param::k_iCallback ); } \
		CCallbackInternal_ ## func ( const CCallbackInternal_ ## func & ) { SteamAPI_RegisterCallback( this, param::k_iCallback ); } \
		CCallbackInternal_ ## func & operator=( const CCallbackInternal_ ## func & ) { return *this; } \
		private: virtual void Run( void *pvParam ) S_OVERRIDE { _STEAM_CALLBACK_AUTO_HOOK( thisclass, func, param ) \
			thisclass *pOuter = reinterpret_cast<thisclass*>( reinterpret_cast<char*>(this) - ((size_t)((char * ) & ((thisclass * ) 0) -> m_fields->m_steamcallback_ ## func)) ); \
			pOuter->func( reinterpret_cast<param*>( pvParam ) ); \
		} \
	}

// Whenever you add STEAM_CALLBACK_FIXED, you need to add this in the fields struct
#define STEAM_CALLBACK_FIELDS(func, param) \
    param m_steamcallback_ ## func \

using namespace geode::prelude;

class $modify(MyGameManager, GameManager) {

    struct Fields {
        SteamAPICall_t m_lobby = NULL;
        uint64 m_lobbyId = 0; // TODO: I probably shouldn't be using 0 as the lobbyID!
        bool m_isInLobby = false;
        
		void onLobbyCreated(LobbyCreated_t *pCallback, bool bIOFailure);
		CCallResult< MyGameManager, LobbyCreated_t > m_isInLobbyCallResult;

		bool m_isHost = true; // TODO: use GetLobbyOwner instead? Possibly?

        STEAM_CALLBACK_FIELDS(onGameJoinRequest, GameLobbyJoinRequested_t);

    };

    // bool init();
    void onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
    STEAM_CALLBACK_FIXED(MyGameManager, onGameJoinRequest, GameLobbyJoinRequested_t);

};
