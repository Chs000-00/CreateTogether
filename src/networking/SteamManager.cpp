#include "SteamManager.hpp"

#ifdef NO_STEAMWORKS
	#include <debug/isteamnetworkingutils.h>
	#include <debug/steamnetworkingsockets.h>
#endif

#ifdef STEAMWORKS

void SteamManager::onGameJoinRequest(GameLobbyJoinRequested_t* pCallback) {


	// Alexa, how do I copy an object from a pointer in c++?
	// Please forgive me.
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
				log::debug("JoinLobbyRequest Called with steamID: {} | friendID: {} | friendName: {}",
					 callback->m_steamIDLobby.ConvertToUint64(),
					 std::hash<uint64_t>{}(callback->m_steamIDFriend.ConvertToUint64()),
					   SteamFriends()->GetFriendPersonaName(callback->m_steamIDFriend)
				);
                
                NetManager::get()->joinSteamLobby(callback);
			}
			delete callback;
		}
		
	);
}

void SteamManager::onLobbyChatUpdateWrapper(LobbyChatUpdate_t* pCallback) {

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

void SteamManager::onLobbyEnter(LobbyEnter_t* pCallback) {

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
		netManager->leaveCurrentSteamLobby();
	}

}


void SteamManager::onLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure) {

	auto netManager = NetManager::get();

	if (pCallback->m_eResult == k_EResultOK) {
		log::info("Created Lobby with steamID {} !", pCallback->m_ulSteamIDLobby);

		FLAlertLayer::create(
			"Lobby host",
			"Lobby created successfully!",
			"Ok"
		)->show();

		// Although this would work, this shouldnt be relied on for checking if
		// the player is in the editor layer.
		// TODO: Change this inside EditorLayer::init instead!
		netManager->m_isInEditorLayer = false;

		netManager->m_lobbyId = pCallback->m_ulSteamIDLobby;

		// Get pointer to the current level editor
		// Because this pointer would be destroyed, we would need to create a new LevelEditorLayer with the same data
		// instead of a pointer, so the LevelEditor would be presistent when leaving the level editor to playtest
		// Currently this is just a temporary solution to get the mod running
		// m_fields->m_level = LevelEditorLayer::get();

		// Constants can be changed in CMakeLists.txt
		// Kind of a bad idea but who cares

		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "lobby_type", MOD_LOBBY_ID);
		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "version", MOD_VERSION);
		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "level_name", LevelEditorLayer::get()->m_level->m_levelName.c_str());
		SteamMatchmaking()->SetLobbyData(pCallback->m_ulSteamIDLobby, "host_name", SteamFriends()->GetPersonaName());
	}
	else {
		log::warn("Failed to create lobby with error code {}!", fmt::underlying(pCallback->m_eResult));
		
		netManager->m_isInEditorLayer = false;
		netManager->m_isInLobby = false;

		this->m_lobbyCreated = 0;
		this->m_lobbyJoined = 0;
	}
}

void SteamManager::onNetworkingMessagesSessionRequest(SteamNetworkingMessagesSessionRequest_t* pCallback) {
	SteamNetworkingMessages()->AcceptSessionWithUser(pCallback->m_identityRemote);
}

#else

void onNetworkingMessagesSessionRequestCallbackFix(SteamNetworkingMessagesSessionRequest_t* pCallback) {
	SteamNetworkingMessages()->AcceptSessionWithUser(pCallback->m_identityRemote);
}


// Note this is only called if NO_STEAMWORKS is defined
SteamManager::SteamManager() {

	SteamDatagramErrMsg errMsg;
	if ( !GameNetworkingSockets_Init( nullptr, errMsg ) ) {
		log::error("GameNetworkingSockets_Init failed {}", errMsg);
	}

	SteamNetworkingUtils()->SetGlobalCallback_MessagesSessionRequest(&onNetworkingMessagesSessionRequestCallbackFix);

	// Allow sharing of any kind of ICE address.
	// We don't have any method of relaying (TURN) in this example, so we are essentially
	// forced to disclose our public address if we want to pierce NAT.  But if we
	// had relay fallback, or if we only wanted to connect on the LAN, we could restrict
	// to only sharing private addresses.
	SteamNetworkingUtils()->SetGlobalConfigValueInt32(k_ESteamNetworkingConfig_P2P_Transport_ICE_Enable, k_nSteamNetworkingConfig_P2P_Transport_ICE_Enable_All );

	this->initP2P();
}

void SteamManager::initP2P() {
	std::vector< SteamNetworkingConfigValue_t > vecOpts;
}


#endif