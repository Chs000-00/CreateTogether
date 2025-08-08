#include <Geode/Geode.hpp>
#include <Geode/modify/LevelOptionsLayer.hpp>
#include "HighLevelHeader.hpp"

using namespace geode::prelude;

class $modify(LevelOptionsLayer) {
    void didToggle(int p0) {
        LevelOptionsLayer::didToggle(p0);

        log::info("Toggle");

        if (!NetManager::getIsInLobby()) {
            return;
        }

        if (p0 == 13) {
            return;
        }

        sendLevelSettingChange(p0);
    }

    void valueDidChange(int p0, float p1) {
        LevelOptionsLayer::valueDidChange(p0, p1);

        if (!NetManager::getIsInLobby()) {
            return;
        }

        if (p0 == 13) {
            sendSpawnGroupChange(p1);
        }
    }
};