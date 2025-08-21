#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/SimplePlayer.hpp>

using namespace geode::prelude;

// Thanks globed!
class CreateTogetherCursor : public cocos2d::CCNode, public cocos2d::CCRGBAProtocol  {
public:

    struct CursorData {
        int cursorColor1;
        int cursorColor2;
        int cursorID;
        bool cursorHasGlow;
        int cursorGlowColor;
    };

    static CreateTogetherCursor* create(const CursorData& cursorData);
    // static CreateTogetherCursor* create(int cursorColor1, int cursorColor2);

    static CursorData defaultCursorData(int cursorColor1, int cursorColor2);

    void updateCursor(const CursorData& cursorData);
    SimplePlayer* getSimplePlayer();

    Ref<SimplePlayer> m_cursorSprite = nullptr;

    // Thanks again globed!
    void setColor(const cocos2d::ccColor3B& color) { m_cursorSprite->setColor(color); }
    const cocos2d::ccColor3B& getColor() { return m_cursorSprite->getColor(); }
    const cocos2d::ccColor3B& getDisplayedColor() { return m_cursorSprite->getDisplayedColor(); }
    GLubyte getDisplayedOpacity() { return m_cursorSprite->getDisplayedOpacity(); }
    GLubyte getOpacity() { return m_cursorSprite->getOpacity(); }
    void setOpacity(GLubyte opacity) { m_cursorSprite->setOpacity(opacity); }
    void setOpacityModifyRGB(bool bValue) { m_cursorSprite->setOpacityModifyRGB(bValue); }
    bool isOpacityModifyRGB() { return m_cursorSprite->isOpacityModifyRGB(); }
    bool isCascadeColorEnabled() { return m_cursorSprite->isCascadeColorEnabled(); }
    void setCascadeColorEnabled(bool cascadeColorEnabled) { m_cursorSprite->setCascadeColorEnabled(cascadeColorEnabled); }
    void updateDisplayedColor(const cocos2d::ccColor3B& color) { m_cursorSprite->updateDisplayedColor(color); }
    bool isCascadeOpacityEnabled() { return m_cursorSprite->isCascadeOpacityEnabled(); }
    void setCascadeOpacityEnabled(bool cascadeOpacityEnabled) { m_cursorSprite->setCascadeOpacityEnabled(cascadeOpacityEnabled); }
    void updateDisplayedOpacity(GLubyte opacity) { m_cursorSprite->updateDisplayedOpacity(opacity); }



protected:
    bool init(const CursorData& cursorData);

    CursorData m_cursorData;

};