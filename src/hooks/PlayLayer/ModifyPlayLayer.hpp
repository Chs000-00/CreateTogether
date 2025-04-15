#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        // Is this a faked play layer? For more info see MyLevelEditorLayer::createFakePlayLayer()
        bool m_isFakeLayer = false;
    };
};