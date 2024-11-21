#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <isteammatchmaking.h>
#include "ModifyGameManager.hpp"


using namespace geode::prelude;

// This is sorta usless...
// I should remove
// bool MyGameManager::init() {

// 	if (!GameManager::init()) {
// 		return false;
// 	}

// 	return true;
// }

void MyGameManager::onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure) {
	if (pCallback->m_eResult == k_EResultOK) {
		m_fields->m_isInLobby = true;
		m_fields->m_isHost = true;
		m_fields->m_lobbyId = pCallback->m_ulSteamIDLobby;

		// Constants can be changed in CMakeLists.txt
		// Kind of a bad idea but who cares

		// SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "lobby_type", MOD_LOBBY_NAME); // TODO: Uncomment this
		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "version", MOD_VERSION);
	}
}



void MyGameManager::onLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure) {
	m_fields->m_lobbyId = pCallback->m_ulSteamIDLobby;
	if (m_fields->m_isHost) {
		log::info("Entered Lobby as host, not doing anything anymore!");
		return;
	}
	else {
		// do stuff here
	}
}

// I am slowly devolving into madness
void MyGameManager::onGameJoinRequest(GameLobbyJoinRequested_t* pCallback) {
	log::debug("Joining game with steamID: {}", pCallback->m_steamIDFriend.ConvertToUint64());
	auto lobby = SteamMatchmaking()->JoinLobby(pCallback->m_steamIDLobby); // I'm not sure if this is needed
}


void MyGameManager::fetchMemberList() {
	int lobbyMemberCount = SteamMatchmaking()->GetNumLobbyMembers(m_fields->m_lobbyId);
	SteamNetworkingIdentity member;

	for ( int i = 0; i < lobbyMemberCount; i++ )
	{
		CSteamID steamIDFriend = SteamMatchmaking()->GetLobbyMemberByIndex(m_fields->m_lobbyId, i);
		member.SetSteamID(steamIDFriend);
		m_fields->m_playersInLobby.push_back(member);   
	} 
}