#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "ModifyPlayLayer.hpp"

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
    void onQuit(CCObject* sender);
    void onEdit(CCObject* sender);
};