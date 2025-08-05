#include "LevelListBorder.hpp"

bool LevelListBorders::init() {
    if (!CCNode::init())
        return false;

    this->setAnchorPoint({ .5f, .5f });
    this->setSpriteFrames("GJ_table_top_001.png", "GJ_table_bottom_001.png", "GJ_table_side_001.png");

    return true;
}

LevelListBorders* LevelListBorders::create() {
    auto ret = new LevelListBorders();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void LevelListBorders::setSpriteFrames(const char* top, const char* bottom, const char* side, float horizontalPadding) {
    this->setSprites(
        CCScale9Sprite::createWithSpriteFrameName(top),
        CCScale9Sprite::createWithSpriteFrameName(bottom),
        CCSprite::createWithSpriteFrameName(side),
        CCSprite::createWithSpriteFrameName(side),
        horizontalPadding,
        horizontalPadding
    );
    // m_bottom->setScaleY(-1);
    m_right->setFlipX(true);
}

void LevelListBorders::setSprites(
    CCScale9Sprite* top, CCScale9Sprite* bottom,
    CCSprite* left, CCSprite* right,
    float topPadding, float bottomPadding
) {
    if (m_top) m_top->removeFromParent();
    if (m_bottom) m_bottom->removeFromParent();
    if (m_left) m_left->removeFromParent();
    if (m_right) m_right->removeFromParent();

    m_top = top;
    this->addChildAtPosition(m_top, Anchor::Top, ccp(0, -m_top->getContentHeight() / 3));

    m_bottom = bottom;
    this->addChildAtPosition(m_bottom, Anchor::Bottom, ccp(0, m_bottom->getContentHeight() / 3));

    m_left = left;
    this->addChildAtPosition(m_left, Anchor::Left, ccp(0, 0));

    m_right = right;
    this->addChildAtPosition(m_right, Anchor::Right, ccp(0, 0));

    m_topPadding = topPadding;
    m_bottomPadding = bottomPadding;

    this->setContentSize(m_obContentSize);
}

void LevelListBorders::setContentSize(CCSize const& size) {
    CCNode::setContentSize(size);
    this->updateLayout();

    m_top->setContentWidth(size.width + m_topPadding + 5);
    m_bottom->setContentWidth(size.width + m_bottomPadding);

    m_top->setZOrder(1);
    m_bottom->setZOrder(1);

    auto height = m_top->getContentHeight() * 0.75 + m_bottom->getContentHeight() * 0.75;
    m_left->setScaleY((size.height - height) / m_left->getContentHeight() + 0.25);
    m_right->setScaleY((size.height - height) / m_right->getContentHeight() + 0.25);
}

// Vertical padding does nothing for nows
void LevelListBorders::setPadding(float horizontalPadding) {
    m_topPadding = horizontalPadding;
    m_bottomPadding = horizontalPadding;

    // m_leftPadding = verticalPadding;
    // m_rightPadding = verticalPadding;
}