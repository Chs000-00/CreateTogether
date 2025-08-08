#include <Geode/Geode.hpp>
#include <Geode/modify/SelectArtLayer.hpp>
#include "HighLevelHeader.hpp"

using namespace geode::prelude;

class $modify(SelectArtLayer) {
    void selectArt(CCObject* sender) {
        SelectArtLayer::selectArt(sender);

        if (!NetManager::getIsInLobby()) {
            return;
        }    
        
        sendChangeArt(this->m_type, this->m_art, this->m_line);

    }
};