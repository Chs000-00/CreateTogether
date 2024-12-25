#include <Geode/Geode.hpp>
#include <Geode/ui/General.hpp>
#include "LevelListBorders.hpp"

using namespace geode::prelude;


void GDLevelListBorders::setPadding(float horizontalPadding) {
    this->setSprites(
        CCScale9Sprite::createWithSpriteFrameName("GJ_table_top_001.png"),
        CCScale9Sprite::createWithSpriteFrameName("GJ_table_bottom_001.png"),
        CCSprite::createWithSpriteFrameName("GJ_table_side_001.png"),
        CCSprite::createWithSpriteFrameName("GJ_table_side_001.png"),
        horizontalPadding + 5.0f,
        horizontalPadding
    );
    m_right->setFlipX(true);
}

bool GDLevelListBorders::init() {
    if (!CCNode::init())
        return false;
    
    this->setAnchorPoint({ .5f, .5f });

    this->setPadding(20.0f);

    m_top->setZOrder(1);
    m_bottom->setZOrder(1);

    
    return true;
}

GDLevelListBorders* GDLevelListBorders::create() {
    auto ret = new GDLevelListBorders();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void GDLevelListBorders::setContentSize(CCSize const& size) {
    CCNode::setContentSize(size);
    this->updateLayout();

    m_top->setContentWidth(size.width + m_topPadding);
    m_bottom->setContentWidth(size.width + m_bottomPadding);
    auto height = m_top->getContentHeight() * 0.75 + m_bottom->getContentHeight() * 0.75;
    m_left->setScaleY(((size.height - height) / m_left->getContentHeight()) + 0.2f);
    m_right->setScaleY(((size.height - height) / m_right->getContentHeight()) + 0.2f);
}
