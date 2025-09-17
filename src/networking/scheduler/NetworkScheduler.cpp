#include "NetworkScheduler.hpp"
#include "../../hooks/ModifyGameManager.hpp"
#include "Geode/loader/Log.hpp"


$execute {
    log::info("Creating CT NetworkScheduler!");
    Loader::get()->queueInMainThread([]{
        CCScheduler::get()->scheduleUpdateForTarget(new NetworkScheduler{}, 5000, false);
    });
}

void NetworkScheduler::update(float dt) {
    if (!m_staticGameManager) {
        this->m_staticGameManager = static_cast<MyGameManager*>(GameManager::get());
    }
    m_staticGameManager->m_fields->m_netManager->update();
}