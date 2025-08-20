#include <Geode/Geode.hpp>
#include <Geode/binding/SimplePlayer.hpp>
#include "Cursor.hpp"
#include "Geode/binding/GameManager.hpp"

using namespace geode::prelude;

CreateTogetherCursor* CreateTogetherCursor::create(int cursorColor1, int cursorColor2) {
    return CreateTogetherCursor::create(CursorData(
        cursorColor1,
        cursorColor2,
        1,
        true,
        0
    ));
}

CreateTogetherCursor* CreateTogetherCursor::create(const CursorData& cursorData) {
    auto ret = new CreateTogetherCursor();
    if (ret->init(cursorData)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void CreateTogetherCursor::updateCursor(const CursorData& cursorData) {
    auto gameManager = GameManager::get();
    this->m_cursorSprite->updatePlayerFrame(cursorData.cursorID, IconType::Wave);
    this->m_cursorSprite->setColor(gameManager->colorForIdx(cursorData.cursorColor1));
    this->m_cursorSprite->setSecondColor(gameManager->colorForIdx(cursorData.cursorColor2));

    if (cursorData.cursorHasGlow) {
        this->m_cursorSprite->setGlowOutline(gameManager->colorForIdx(cursorData.cursorGlowColor));
    }
    else {
        this->m_cursorSprite->disableGlowOutline();
    }
}

SimplePlayer* CreateTogetherCursor::getSimplePlayer() {
    return this->m_cursorSprite;
}

bool CreateTogetherCursor::init(const CursorData& cursorData) {
    if (!CCNode::init()) {
        return false;
    }
    this->m_cursorSprite = SimplePlayer::create(0);
    this->updateCursor(cursorData);
    return true;
}
