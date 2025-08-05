#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/General.hpp>

using namespace geode::prelude;

class LevelListBorders : public cocos2d::CCNode {
protected:
    cocos2d::extension::CCScale9Sprite* m_top = nullptr;
    cocos2d::extension::CCScale9Sprite* m_bottom = nullptr;
    cocos2d::CCSprite* m_left = nullptr;
    cocos2d::CCSprite* m_right = nullptr;
    float m_topPadding = 7.5f;
    float m_bottomPadding = 7.5f;

    float m_leftPadding = 0.0f;
    float m_rightPadding = 0.0f;

    bool init() override;

public:
    static LevelListBorders* create();

    void setSpriteFrames(const char* top, const char* bottom, const char* side, float horizontalPadding = 7.5f);
    void setSprites(
        cocos2d::extension::CCScale9Sprite* top,
        cocos2d::extension::CCScale9Sprite* bottom,
        cocos2d::CCSprite* left,
        cocos2d::CCSprite* right,
        float topPadding = 7.5f,
        float bottomPadding = 7.5f
    );
    void setContentSize(cocos2d::CCSize const& size) override;
    void setPadding(float horizontalPadding);
};
