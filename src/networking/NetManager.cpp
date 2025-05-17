#include "NetManager.hpp"
#include "../hooks/ModifyGameManager.hpp"
NetManager* NetManager::get() {
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    return gameManager->m_fields->m_netManager;
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

void NetManager::sendMessageHeaderToUser(SteamNetworkingIdentity usr, flatbuffers::FlatBufferBuilder* out) {
	#ifndef USE_TEST_SERVER
		SteamNetworkingMessages()->SendMessageToUser(usr, out->GetBufferPointer(), out->GetSize(), k_nSteamNetworkingSend_Reliable, 0);
	#else
        log::warn("Attempted to sendMessageHeaderToUser yet server is dedicated! Ignoring.");
    #endif
}

void NetManager::sendData(flatbuffers::FlatBufferBuilder* out) {


	// log::info("Sending MSG {} {}", data, static_cast<uint32>(strlen(data.c_str())));

	#ifndef USE_TEST_SERVER        

		for (auto const& member : this->m_playersInLobby) {
			this->sendMessageHeaderToUser(member, out);
		}
		// log::debug("Done sending messages");

	#else
		send(this->m_fields->m_socket, data.c_str(), strlen(data.c_str()), 0);
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
