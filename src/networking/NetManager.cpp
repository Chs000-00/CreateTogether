#include "../hooks/ModifyGameManager.hpp"
#include "NetManager.hpp"
#include "./highlevel/NetRecv.hpp"
#include "../utils/Utills.hpp"


// Why the f*** can't c++ inline this f***ing shit properly without some stupid use of header guards
NetManager* NetManager::get() {
	auto gameManager = static_cast<MyGameManager*>(GameManager::get());
	return gameManager->m_fields->m_netManager;
}

bool NetManager::getIsInLobby() {
	return NetManager::get()->m_isInEditorLayer;
}

void NetManager::update() {
    SteamAPI_RunCallbacks();

	if (this->m_isInLobby) {
		this->receiveData();
		this->sendQueuedData();
	}
}

void NetManager::leaveLobby() {
	this->m_isInEditorLayer = false;

	// Clear buffer so it dosen't cause issues later.
	this->m_builder.Clear();
	if (this->m_isInLobby) {

        m_isInLobby = false;

		#ifdef STEAMWORKS

			log::info("Leaving lobby with ID {}", this->m_lobbyId);
			SteamMatchmaking()->LeaveLobby(this->m_lobbyId);
			this->m_lobbyId = 0;

		#else

			
		#endif

		// this->m_lobbyCreated = 0;
		// this->m_lobbyJoined = 0;

	}
	else {
		log::info("Can't leave lobby because not in lobby!");
	}
}

void NetManager::sendMessageToUser(SteamNetworkingIdentity usr, flatbuffers::Offset<CTSerialize::MessageHeader> out) {
	this->m_builder.Finish(out);
	SteamNetworkingMessages()->SendMessageToUser(usr, this->m_builder.GetBufferPointer(), this->m_builder.GetSize(), k_nSteamNetworkingSend_Reliable, 0);
}

void NetManager::sendMessage(flatbuffers::Offset<CTSerialize::MessageHeader> out) {

	this->m_builder.Finish(out);
	// log::info("Sending MSG {} {}", data, static_cast<uint32>(strlen(data.c_str())));


	for (auto const& member : this->m_playersInLobby) {
		SteamNetworkingMessages()->SendMessageToUser(member, this->m_builder.GetBufferPointer(), this->m_builder.GetSize(), k_nSteamNetworkingSend_Reliable, 0);
	}
	// log::debug("Done sending messages");
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

	// I AM VERY SORRY FOR THIS ):
	flatbuffers::FlatBufferBuilder builder;

	SteamNetworkingIdentity host;
	host.SetSteamID(this->m_hostID);

	std::vector<uint8_t> bodyType;
	bodyType.push_back(CTSerialize::MessageBody_RequestLevel);
	auto bodyTypeOffset = builder.CreateVector(bodyType);

	auto requestLevelStringOffset = CTSerialize::CreateRequestLevel(builder);
	std::vector<flatbuffers::Offset<void>> body;
	body.push_back(requestLevelStringOffset.Union());
	auto bodyOffset = builder.CreateVector(body);

	auto messageHeader = CTSerialize::CreateMessageHeader(builder, bodyTypeOffset, bodyOffset);

	this->sendMessageToUser(host, messageHeader);

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

	SteamNetworkingMessage_t* messageList[MAX_MESSAGES];
	auto numMessages = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, messageList, MAX_MESSAGES);

	if (numMessages < 0) {
		log::warn("NetManager::receiveData(): Error receiving messages");
	}

	for (int i = 0; i < numMessages; i++) {
		
		SteamNetworkingMessage_t* msg = messageList[i];

		// Uhh idk anymore what this is
		// This should create a msg->GetSize() sized data object.
		const uint8_t* data = new uint8_t[msg->GetSize()];
		data = static_cast<const uint8_t*>(msg->GetData());
		
		auto messageHeader = CTSerialize::GetMessageHeader(data);

		
		
		flatbuffers::Verifier verifier(data, (size_t)msg->GetData());

		bool isVerified = CTSerialize::VerifyMessageHeaderBuffer(verifier);


		auto out = this->parseData(messageHeader);

		if (!out) {
			log::warn("Something went wrong while parsing: {}", out.unwrapErr());
		}

		delete data;
		msg->Release();
	}
}

void NetManager::sendQueuedData() {

	auto bodyTypeOffset = this->m_builder.CreateVector(this->m_bodyType);
	auto bodyOffset = this->m_builder.CreateVector(this->m_body);

	auto messageHeader = CTSerialize::CreateMessageHeader(this->m_builder, bodyTypeOffset, bodyOffset);

	this->sendMessage(messageHeader);
	this->m_builder.Clear();
}

        
Result<uint8_t> NetManager::parseData(const CTSerialize::MessageHeader* msg) {
	auto messageUnionType = msg->body_type();
	for (auto i = 0; i < messageUnionType->Length(); i++) {

		auto dmsg = msg->body()->Get(i);

		switch (messageUnionType->Get(i)) {
			case CTSerialize::MessageBody_CreateObjects: {
				SERIALIZE_AND_RECEIVE(CreateObjects);
				break;
			}

			default:
				return Err("Invalid Union Type");
		}
	}
	return Ok(0);
}

#ifdef USE_STEAMWORKS
void NetManager::joinSteamLobby(GameLobbyJoinRequested_t* lobbyInfo) {
	// TODO: Remove this
	log::debug("JoinLobbyRequest Called with steamID: {} | friendID: {} | friendName: {}", lobbyInfo->m_steamIDLobby.ConvertToUint64(), lobbyInfo->m_steamIDFriend.ConvertToUint64(), SteamFriends()->GetFriendPersonaName(lobbyInfo->m_steamIDFriend));
	this->m_isInLobby = SteamMatchmaking()->JoinLobby(lobbyInfo->m_steamIDLobby); // I'm not sure if this is needed
}
#endif
