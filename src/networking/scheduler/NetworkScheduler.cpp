#include "NetworkScheduler.hpp"
#include "../../hooks/ModifyGameManager.hpp"


$execute {
    Loader::get()->queueInMainThread([]{
        CCScheduler::get()->scheduleUpdateForTarget(NetworkScheduler::create(), 5000, false);
    });
}

void NetworkScheduler::update() {
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    gameManager->m_fields->m_netManager->update();
}


NetworkScheduler* NetworkScheduler::create() {
    auto ret = new NetworkScheduler();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool NetworkScheduler::init() {
    if (!CCNode::init()) {
        return false;
    }
    return true;
}
