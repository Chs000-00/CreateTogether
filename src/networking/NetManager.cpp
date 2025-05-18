#include "NetManager.hpp"
#include "../hooks/ModifyGameManager.hpp"
#include "../ui/WaitingForHostPopup.hpp"

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



void NetManager::enterLevelEditor() {
	WaitingForHostPopup::create();
	this->m_isRequestingLevelString = true;
	this->m_isInEditorLayer = false;
	auto gameLevel = GJGameLevel::create();
	gameLevel->m_isEditable = true;
	gameLevel->m_levelType = GJLevelType::Editor;
	// gameLevel->m_levelDesc += "Created with Create Together";
	auto lev = LevelEditorLayer::create(gameLevel, false);
	// TODO: Only send this to host!!!!!!!!!
	auto msg = "{\"Type\": 7}";


    #ifndef USE_TEST_SERVER
        SteamNetworkingIdentity host;
        host.SetSteamID(this->m_hostID);
        SteamNetworkingMessages()->SendMessageToUser(host, msg, static_cast<uint32>(strlen(msg)), k_nSteamNetworkingSend_Reliable, 0);
    #else
		// FIX! THIS!
        sendDataToMembers(msg);
    #endif

	this->fetchMemberList();
	switchToScene(lev);
}

void NetManager::fetchMemberList() {
	int lobbyMemberCount = SteamMatchmaking()->GetNumLobbyMembers(this->m_lobbyId);
	SteamNetworkingIdentity member;

	log::debug("FetchMemberList Called; Fetching {} users:", lobbyMemberCount);

	for (int i = 0; i < lobbyMemberCount; i++) {
		CSteamID steamIDLobbyMember = SteamMatchmaking()->GetLobbyMemberByIndex(this->m_lobbyId, i);
		if (steamIDLobbyMember == SteamUser()->GetSteamID()) {
			log::debug("(Self) - {} with SteamID: {}", SteamFriends()->GetPersonaName(), steamIDLobbyMember.ConvertToUint64());
			continue;
		}

		member.SetSteamID(steamIDLobbyMember);
		log::debug("{} with SteamID: {}", SteamFriends()->GetFriendPersonaName(steamIDLobbyMember), steamIDLobbyMember.ConvertToUint64());
		this->m_playersInLobby.push_back(member);   
	} 
}


void NetManager::receiveData() {

	if (this->m_isInEditorLayer) {
		return;
	}

	#ifndef USE_TEST_SERVER
		SteamNetworkingMessage_t* messageList[128];
		auto numMessages = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, messageList, 128);
	#else
		// Compat with for loop
		auto numMessages = 1;

		// What is this used for?
		// char tserverdat[8192];
		// memset(tserverdat, 0, sizeof(tserverdat)); 
		TestServerMsg* msg = new TestServerMsg;
		auto outrec = recv(this->m_socket, msg->m_data, sizeof(msg->m_data), 0);

		if (outrec == -1) {
			msg->Release();
			return;
		}

	#endif

	for (int i = 0; i < numMessages; i++) {
		
		#ifndef USE_TEST_SERVER
			SteamNetworkingMessage_t* msg = messageList[i];
		#endif

		// Uhh idk anymore what this is
		const char* data = new char[msg->GetSize()];
		data = static_cast<const char*>(msg->GetData());
		
		CTSerialize::GetMessageHeader(data);

		auto out = this->parseData();

		if (!out) {
			log::warn("Something went wrong with the parsing: {}", out.unwrapErr());
		}

		delete data;
		data = nullptr;	// just in case
		msg->Release();
	}
}