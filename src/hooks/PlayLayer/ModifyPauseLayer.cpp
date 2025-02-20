#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "ModifyPlayLayer.hpp"
#include "ModifyPauseLayer.hpp"


using namespace geode::prelude;

void MyPauseLayer::onQuit(CCObject* sender) {
    // auto playLayer = static_cast<MyPlayLayer*>(PlayLayer::get())
    PauseLayer::onQuit(sender);
}

void MyPauseLayer::onEdit(CCObject* sender) {
    PauseLayer::onEdit(sender);
}