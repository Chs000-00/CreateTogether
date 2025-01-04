#include <Geode/Geode.hpp>
#include <Geode/modify/SelectArtLayer.hpp>
#include "../ModifyGameManager.hpp"
#include "../../types/ActionTypes.hpp"

using namespace geode::prelude;

class $modify(SelectArtLayer) {
    void selectArt(CCObject* sender) {
        auto gameManager = static_cast<MyGameManager*>(GameManager::get());

        if (!gameManager->m_fields->m_isInLobby) {
            SelectArtLayer::selectArt(sender);
            return;
        }

        matjson::Value object = matjson::makeObject({
            {"Type", static_cast<int>(eActionArtSelected)},
            {"ArtType", static_cast<int>(this->m_type)},
            {"Art", this->m_art},
            {"Line", this->m_line}
        });


        gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));
        SelectArtLayer::selectArt(sender);
    }
};
