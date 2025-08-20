#include "NetworkScheduler.hpp"
#include "../../hooks/ModifyGameManager.hpp"
#include "Geode/loader/Log.hpp"


$execute {
    log::info("Hello NetworkScheduler!");
    Loader::get()->queueInMainThread([]{
        log::info("Hello from mainthread!");
        CCScheduler::get()->scheduleUpdateForTarget(new NetworkScheduler{}, 5000, false);
    });
}

void NetworkScheduler::update(float dt) {
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    gameManager->m_fields->m_netManager->update();
}