#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <isteamfriends.h>
#include <steam_api_common.h>
#include <steamnetworkingtypes.h>

// Fixed version of STEAM_CALLBACK.
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

// For STEAM_CALLBACK_FIXED to work properly, you need to add this into struct fields.
#define STEAM_CALLBACK_FIELDS(func, param) \
    param m_steamcallback_ ## func \

using namespace geode::prelude;

class $modify(MyGameManager, GameManager) {

    struct Fields {
		// Useless field for a callback. Should try removing.
        SteamAPICall_t m_lobby = NULL;
        uint64 m_lobbyId = 0; // TODO: I probably shouldn't be using 0 as the lobbyID!
        bool m_isInLobby = false;
		bool m_isHost = false; // TODO: use GetLobbyOwner instead? Possibly?

		std::vector<SteamNetworkingIdentity> m_playersInLobby;

        STEAM_CALLBACK_FIELDS(onGameJoinRequest, GameLobbyJoinRequested_t);

		CCallResult< MyGameManager, LobbyCreated_t > m_isInLobbyCallResult;
		CCallResult< MyGameManager, LobbyEnter_t > m_enterLobbyCallResult;

    };

    // bool init();
    void onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
	void onLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure);
	void fetchMemberList();
    STEAM_CALLBACK_FIXED(MyGameManager, onGameJoinRequest, GameLobbyJoinRequested_t);

};
