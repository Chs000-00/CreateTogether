#include <Geode/Geode.hpp>
#include <Geode/binding/SimplePlayer.hpp>
#include "Cursor.hpp"

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

}

SimplePlayer* CreateTogetherCursor::getSimplePlayer() {
    return this->m_cursorSprite;
}

bool CreateTogetherCursor::init(const CursorData& cursorData) {
    if (!CCNode::init()) {
        return false;
    }

    this->updateCursor(cursorData);
    return true;
}
