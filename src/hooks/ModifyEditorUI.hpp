#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include "ModifyGameManager.hpp"

using namespace geode::prelude;


class $modify(MyEditorUI, EditorUI) {

    // struct Fields {
    //     // Was the data sent from another user?
    //     bool m_wasDataSent = false;
    // };

    void onDeleteSelected(CCObject* sender);
};