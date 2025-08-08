#include <Geode/Geode.hpp>
#include <Geode/modify/SongSelectNode.hpp>
#include "HighLevelHeader.hpp"

using namespace geode::prelude;

class $modify(SongSelectNode) {
    void songIDChanged(int p0) {
        SongSelectNode::songIDChanged(p0);

        if (!NetManager::getIsInLobby()) {
            return;
        }

        sendUpdateSong(p0);
    }
    
    void audioNext(CCObject* sender) {
        SongSelectNode::audioNext(sender);

        if (!NetManager::getIsInLobby()) {
            return;
        }

        sendUpdateSong(m_selectedSongID);
    }

    void audioPrevious(CCObject* sender) {
        SongSelectNode::audioPrevious(sender);

        if (!NetManager::getIsInLobby()) {
            
            return;
        }

        sendUpdateSong(m_selectedSongID);
    }


    void onSongMode(int mode) {
        SongSelectNode::onSongMode(mode);

        if (!NetManager::getIsInLobby()) {
            return;
        }

        sendUpdateSong(m_selectedSongID);
    }
};