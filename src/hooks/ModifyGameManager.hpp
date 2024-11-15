#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>




using namespace geode::prelude;

class $modify(MyGameManager, GameManager) {

    struct Fields {
        SteamAPICall_t m_lobby = NULL;
        uint64 m_lobbyId = 0; // TODO: I probably shouldn't be using 0 as the lobbyID!
        bool m_isInLobby = false;
        
		void onLobbyCreated(LobbyCreated_t *pCallback, bool bIOFailure);
		CCallResult< MyGameManager, LobbyCreated_t > m_isInLobbyCallResult;

		bool m_isHost = true; // TODO: use GetLobbyOwner instead
    };

    bool init();

	void onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
};
