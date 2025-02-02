#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include "ModifyGameManager.hpp"

using namespace geode::prelude;




class $modify(MyEditorUI, EditorUI) {

    struct Fields {
        // Was the data sent from another user?
        bool m_wasDataSent = false;
        bool m_loadingFinished = false;
    };

    bool init(LevelEditorLayer* editorLayer);

    void transformObject(GameObject* p0, EditCommand p1, bool p2);
    void onDeleteSelected(CCObject* sender);

    // TODO: Figure out moveObjectCall
    void moveObject(GameObject* p0, CCPoint p1);

    void removeSelectedObjects();
    void deleteSingleObject(GameObject* dObj);
    // gd::string copyObjects(CCArray* objects, bool copyColors, bool sort);
    CCArray* pasteObjects(gd::string p0, bool p1, bool p2);
    matjson::Value removeSelectedObjectsWithMatjson();
};