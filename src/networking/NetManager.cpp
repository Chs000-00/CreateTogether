#include "NetManager.hpp"

NetManager* NetManager::get() {
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    return gameManager->m_fields->m_networkManager;
}

void NetManager::update() {
    SteamAPI_RunCallbacks();

	if (this->m_fields->m_isInLobby) {
		this->receiveData();
		this->sendQueuedData();
	}
}