#include <Geode/Geode.hpp>
#include <Geode/modify/LevelOptionsLayer.hpp>
#include "../ModifyGameManager.hpp"
#include "../../types/ActionTypes.hpp"

#include <Geode/loader/Dirs.hpp>


using namespace geode::prelude;

// TODO: Implement
// Looks like this function is called anytime a toggle setting gets changed, like "Start as duel"
class $modify(LevelOptionsLayer) {
    void didToggle(int p0) {

        log::info("LevelOptionsLayer: p0 {}", p0);
        
        LevelOptionsLayer::didToggle(p0);

        auto gameManager = static_cast<MyGameManager*>(GameManager::get());

        if (!gameManager->m_fields->m_isInLobby) {
            return;
        }

        matjson::Value object = matjson::makeObject({
            {"Type", static_cast<int>(eOptionLevelSetting)},
            {"ToggleIndex", p0}
        });


        gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));

    }
};
