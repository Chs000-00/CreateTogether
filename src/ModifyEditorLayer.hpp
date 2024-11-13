#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>


using namespace geode::prelude;

class $modify(MyLevelEditorLayer, LevelEditorLayer) {

    struct Fields {
        SteamAPICall_t m_lobby = NULL;
        uint64 m_lobbyId = 0;
        bool m_isInLobby = false;
        
		void onLobbyCreated(LobbyCreated_t *pCallback, bool bIOFailure);
		CCallResult< MyLevelEditorLayer, LobbyCreated_t > m_isInLobbyCallResult;

		bool m_isHost = true; // TODO: use GetLobbyOwner instead
    };

    bool init(GJGameLevel* p0, bool p1);

	void onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
};
