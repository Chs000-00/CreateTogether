#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <isteammatchmaking.h>
#include "ModifyGameManager.hpp"


using namespace geode::prelude;

// This is sorta usless...
// I should remove
bool MyGameManager::init() {

	if (!GameManager::init()) {
		return false;
	}

	return true;
}

void MyGameManager::onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure) {
	if (pCallback->m_eResult == k_EResultOK) {
		m_fields->m_isInLobby = true;
		m_fields->m_lobbyId = pCallback->m_ulSteamIDLobby;

		// Constants can be changed in CMakeLists.txt
		// Kind of a bad idea but who cares

		// SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "lobby_type", MOD_LOBBY_NAME); // TODO: Uncomment this
		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "version", MOD_VERSION);
	}
}


