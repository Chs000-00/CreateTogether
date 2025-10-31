#include "NetworkScheduler.hpp"
#include "Geode/loader/Log.hpp"
#include "NetManager.hpp"


$execute {
    log::info("Creating CT NetworkScheduler!");
    Loader::get()->queueInMainThread([]{
        CCScheduler::get()->scheduleUpdateForTarget(new NetworkScheduler{}, 5000, false);
    });
}

void NetworkScheduler::update(float dt) {
    NetManager::get()->update();
}