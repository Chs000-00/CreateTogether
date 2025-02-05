#include <Geode/Geode.hpp>
#include <Geode/modify/LevelOptionsLayer.hpp>
#include "../ModifyGameManager.hpp"
#include "../../types/ActionTypes.hpp"

using namespace geode::prelude;

// TODO: Implement
// Looks like this function is called anytime a toggle setting gets changed, like "Start as duel"
class $modify(LevelOptionsLayer) {
    void didToggle(int p0) {
        LevelOptionsLayer::didToggle(p0);
    }
};
