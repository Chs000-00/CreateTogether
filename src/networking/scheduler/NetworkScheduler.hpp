#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;


class NetworkScheduler : public CCNode {
public:
    void update();

    static NetworkScheduler* create();

protected:
    bool init();
};

