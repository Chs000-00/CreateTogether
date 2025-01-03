#include <Geode/Geode.hpp>
#include <Geode/modify/SongSelectNode.hpp>
#include "../ModifyGameManager.hpp"
#include "../../types/ActionTypes.hpp"

using namespace geode::prelude;

class $modify(SongSelectNode) {
    void songIDChanged(int p0) {
        auto gameManager = static_cast<MyGameManager*>(GameManager::get());

        if (!gameManager->m_fields->m_isInLobby) {
            SongSelectNode::songIDChanged(p0);
            return;
        }

        matjson::Value object = matjson::makeObject({
            {"Type", static_cast<int>(eActionSongChanged)},
            {"Custom", true},
            {"SongID", p0}
        });

        log::info("Song: {}", p0);

        gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));
        SongSelectNode::songIDChanged(p0);
    }
};
