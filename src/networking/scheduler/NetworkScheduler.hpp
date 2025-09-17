#pragma once

#include <Geode/Geode.hpp>
#include "../../hooks/ModifyGameManager.hpp"


using namespace geode::prelude;


class NetworkScheduler : public CCObject {
public:
    MyGameManager* m_staticGameManager = nullptr;

    void update(float dt);
};

