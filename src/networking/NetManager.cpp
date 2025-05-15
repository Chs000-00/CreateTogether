#include "NetManager.hpp"
#include "../hooks/ModifyGameManager.hpp"

NetManager* NetManager::get() {
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    return gameManager->m_fields->m_networkManager;
}

void NetManager::update() {
    SteamAPI_RunCallbacks();

	if (this->m_isInLobby) {
		this->receiveData();
		this->flushQueue();
	}
}

void NetManager::leaveCurrentSteamLobby() {
	this->m_isInEditorLayer = false;
	if (this->m_isInLobby) {

        m_isInLobby = false;

		#ifndef USE_TEST_SERVER

			log::info("Leaving lobby with ID {}", this->m_lobbyId);
			SteamMatchmaking()->LeaveLobby(this->m_lobbyId);
			this->m_lobbyId = 0;

		#else

			// WHY DOES THIS CRASH????
			close(this->m_socket);
			
		#endif

		// this->m_lobbyCreated = 0;
		// this->m_lobbyJoined = 0;

	}
	else {
		log::info("Can't leave lobby because not in lobby!");
	}
}

void NetManager::sendMessageHeaderToUser(SteamNetworkingIdentity usr, CTSerialize::MessageHeader out) {
	#ifndef USE_TEST_SERVER
		SteamNetworkingMessages()->SendMessageToUser(usr, out, static_cast<uint32>(sizeof(out)), k_nSteamNetworkingSend_Reliable, 0);
	#else
        // this->sendDataToMembers(out, false);
    #endif
}


void NetManager::flushQueue() {
	if (this->m_massEdit.m_sendGroupIDEdits) {

		// matjson::Value object = matjson::makeObject({
		// 	{"Type", static_cast<int>(eActionChangeGroupID)},
		// 	{"Add", this->m_massEdit.m_isAddingGroupID},
		// 	{"GroupID", this->m_massEdit.m_groupIDToEdit},
		// 	{"EditUUIDs", this->m_massEdit.m_groupIDEdits}
		// });

		//this->sendDataToMembers(object.dump(matjson::NO_INDENTATION));
	
		this->m_massEdit.m_sendGroupIDEdits = false;
	}
}
