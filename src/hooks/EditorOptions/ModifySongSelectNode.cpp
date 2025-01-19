#include <Geode/Geode.hpp>
#include <Geode/modify/SongSelectNode.hpp>
#include "../ModifyGameManager.hpp"
#include "../../types/ActionTypes.hpp"

using namespace geode::prelude;

class $modify(SongSelectNode) {
    void songIDChanged(int p0) {
        auto gameManager = static_cast<MyGameManager*>(GameManager::get());

        SongSelectNode::songIDChanged(p0);

        if (!gameManager->m_fields->m_isInLobby) {
            return;
        }

        this->sendSong(p0, gameManager);
    }
    
    void audioNext(CCObject* sender) {
        auto gameManager = static_cast<MyGameManager*>(GameManager::get());

        SongSelectNode::audioNext(sender);

        if (!gameManager->m_fields->m_isInLobby) {
            return;
        }

        this->sendSong(m_selectedSongID, gameManager);

    }

    void audioPrevious(CCObject* sender) {
        auto gameManager = static_cast<MyGameManager*>(GameManager::get());

        SongSelectNode::audioPrevious(sender);

        if (!gameManager->m_fields->m_isInLobby) {
            
            return;
        }

        this->sendSong(m_selectedSongID, gameManager);
    }


    void onSongMode(int mode) {
        auto gameManager = static_cast<MyGameManager*>(GameManager::get());

        SongSelectNode::onSongMode(mode);

        if (!gameManager->m_fields->m_isInLobby) {
            return;
        }

        this->sendSong(m_selectedSongID, gameManager);
    }

    void sendSong(int songID, MyGameManager* gameManager) {

        log::info("Song: {} ", songID);
        
        matjson::Value object = matjson::makeObject({
            {"Type", static_cast<int>(eActionSongChanged)},
            {"SongID", songID}
        });

        gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));
    }
};
