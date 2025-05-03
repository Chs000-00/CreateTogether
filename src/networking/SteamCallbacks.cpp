#include "SteamCallbacks.hpp"


void SteamCallbacks::onGameJoinRequest(GameLobbyJoinRequested_t* pCallback) {


	// Alexa, how do I copy an object from a pointer in c++?
	auto callback = new GameLobbyJoinRequested_t;
	callback->m_steamIDFriend = pCallback->m_steamIDFriend;
	callback->m_steamIDLobby = pCallback->m_steamIDLobby;

	geode::createQuickPopup(
		"Lobby",         
		"Join Lobby?",
		"Nah", "Yeah",
		[callback](auto, bool btn2) {
			if (btn2) {
				// TODO: Remove this
				log::debug("JoinLobbyRequest Called with steamID: {} | friendID: {} | friendName: {}", callback->m_steamIDLobby.ConvertToUint64(), callback->m_steamIDFriend.ConvertToUint64(), SteamFriends()->GetFriendPersonaName(callback->m_steamIDFriend));
                
                NetManager::get()->joinSteamLobby(callback->m_steamIDLobby);
			}
			delete callback;
		}
		
	);
}

void SteamCallbacks::onLobbyChatUpdateWrapper(LobbyChatUpdate_t* pCallback) {

    auto netManager = NetManager::get();
	if (netManager->m_lobbyId != pCallback->m_ulSteamIDLobby) {
		return;
	}

	if (pCallback->m_ulSteamIDUserChanged == netManager->m_hostID.ConvertToUint64()) {
		if (pCallback->m_rgfChatMemberStateChange == k_EChatMemberStateChangeLeft || pCallback->m_rgfChatMemberStateChange == k_EChatMemberStateChangeDisconnected) {
			log::info("Host left server! Leaving lobby.");

			LobbiesLayer::scene();
			netManager->leaveCurrentSteamLobby();

			FLAlertLayer::create(
				"Host left server",    
				"The host has left the server!",  
				"Ok"
			)->show();

		}
	}

	log::debug("LobbyChatUpdateWrapper called. UserID: {} | UserName: {} | StateChange: {} | SteamIDMakingChange: {}", pCallback->m_ulSteamIDUserChanged, SteamFriends()->GetFriendPersonaName(pCallback->m_ulSteamIDUserChanged), pCallback->m_rgfChatMemberStateChange, pCallback->m_ulSteamIDMakingChange);


	netManager->fetchMemberList();
}

void SteamCallbacks::onNetworkingMessagesSessionRequest(SteamNetworkingMessagesSessionRequest_t* pCallback) {
	SteamNetworkingMessages()->AcceptSessionWithUser(pCallback->m_identityRemote);
}


void SteamCallbacks::onLobbyEnter(LobbyEnter_t* pCallback) {

	if (pCallback->m_EChatRoomEnterResponse != k_EChatRoomEnterResponseSuccess) {
		log::error("Failed to enter lobby with error code {}", pCallback->m_EChatRoomEnterResponse);
		FLAlertLayer::create(
			"Lobby Error",  
			fmt::format("Failed to enter lobby; <cr>Error {} </c>", pCallback->m_EChatRoomEnterResponse),
			"Ok"   
		)->show();
		return;
	}


	auto netManager = NetManager::get();
	log::info("EnterLobby called with! SteamID: {} | ChatRoomEnterResponse: {}", pCallback->m_ulSteamIDLobby, pCallback->m_EChatRoomEnterResponse);

	netManager->m_isInLobby = true;
	

	if (netManager->m_isHost) {
		log::info("onLobbyEntered called as host!");
	} else {
		netManager->m_lobbyId = pCallback->m_ulSteamIDLobby;
		netManager->enterLevelEditor();
	}

	netManager->m_hostID = SteamMatchmaking()->GetLobbyOwner(netManager->m_lobbyId);

	// Make sure we didn't become host of a level who's host already left.
	if (!netManager->m_isHost && netManager->m_hostID == SteamUser()->GetSteamID()) {
		log::warn("Invalid host! onLobbyEntered was not called as host yet hostID is your steamID");
		log::warn("This is a rare error (:");
		netManager->leaveLobby();
	}

}