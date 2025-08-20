#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;


class NetworkScheduler : public CCObject {
public:
    void update(float dt);
};

