#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include "ModifyGameManager.hpp"
#include "ModifyEditorLayer.hpp"
#include "ModifyGameObject.hpp"
#include "ModifyEditorUI.hpp"
#include "../types/ActionTypes.hpp"

using namespace geode::prelude;


// TODO: test out shouldDeleteObject
void MyEditorUI::onDeleteSelected(CCObject* sender) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

    if (this->m_fields->m_wasDataSent) {
        EditorUI::onDeleteSelected(sender);
        return;
    }

    if (this->m_selectedObject) {
        this->deleteSingleObject(this->m_selectedObject);
        EditorUI::onDeleteSelected(sender);
    }

    auto editUUID = removeSelectedObjectsWithMatjson();

    if (!gameManager->m_fields->m_isInLobby) {
        EditorUI::onDeleteSelected(sender);
        return;
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionDeletedObject)},
        {"EditUUIDs", editUUID},
    });

    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));
    EditorUI::onDeleteSelected(sender);
}

// Possibly unused
void MyEditorUI::removeSelectedObjects() {
    auto editor = static_cast<MyLevelEditorLayer*>(this->m_editorLayer);

    for (auto obj : CCArrayExt<MyGameObject*>(this->m_selectedObjects)) {
        editor->m_fields->m_pUniqueIDOfGameObject->removeObjectForKey(obj->m_fields->m_veryUniqueID);
    }
}

void MyEditorUI::deleteSingleObject(GameObject* dObj) {
    auto editUUID = matjson::Value::array();
    auto obj = static_cast<MyGameObject*>(dObj);
    editUUID.push(obj->m_fields->m_veryUniqueID);
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionDeletedObject)},
        {"EditUUIDs", editUUID},
    });

    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));
}

matjson::Value MyEditorUI::removeSelectedObjectsWithMatjson() {
    auto editor = static_cast<MyLevelEditorLayer*>(this->m_editorLayer);
    auto ret = matjson::Value::array();
    for (auto obj : CCArrayExt<MyGameObject*>(this->m_selectedObjects)) {
        editor->m_fields->m_pUniqueIDOfGameObject->removeObjectForKey(obj->m_fields->m_veryUniqueID);
        ret.push(obj->m_fields->m_veryUniqueID);
    }
    return ret;
}

void MyEditorUI::transformObject(GameObject* p0, EditCommand p1, bool p2) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    auto betterObject = static_cast<MyGameObject*>(p0);

    if (!gameManager->m_fields->m_isInLobby || this->m_fields->m_wasDataSent) {
        EditorUI::transformObject(p0, p1, p2);
        return;
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionTransformObject)},
        {"EditCommand", static_cast<int>(p1)},
        {"ObjectUID", betterObject->m_fields->m_veryUniqueID}
    });
    
    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));

    EditorUI::transformObject(p0, p1, p2);
}

// TODO: Figure out a better way to move objects
void MyEditorUI::moveObject(GameObject* p0, CCPoint p1) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    auto betterObject = static_cast<MyGameObject*>(p0);

    EditorUI::moveObject(p0, p1);

    if (!gameManager->m_fields->m_isInLobby || this->m_fields->m_wasDataSent || !this->m_fields->m_loadingFinished) {
        return;
    }


    if (!gameManager->m_fields->m_sharedMassEdit.m_sendMoveList) {
        gameManager->m_fields->m_sharedMassEdit.m_sendMoveList = true;
        gameManager->m_fields->m_sharedMassEdit.m_moveList = matjson::Value();
    }

    // IDFK Why im doing this
    matjson::Value pos = matjson::Value();
    pos.set("x", p0->getPositionX());
    pos.set("y", p0->getPositionY());
    gameManager->m_fields->m_sharedMassEdit.m_moveList[betterObject->m_fields->m_veryUniqueID] = pos;


    // matjson::Value object = matjson::makeObject({
    //     {"Type", static_cast<int>(eActionMovedObject)},
    //     {"x", p0->getPositionX()},
    //     {"y", p0->getPositionY()},
    //     {"ObjectUID", betterObject->m_fields->m_veryUniqueID}
    // });

    // gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION).c_str());
}

bool MyEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) {
        return false;
    }
    this->m_fields->m_loadingFinished = true;
    return true;
}


CCArray* MyEditorUI::pasteObjects(gd::string p0, bool p1, bool p2) {
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

    CCArray* ret = EditorUI::pasteObjects(p0, p1, p2);

    auto objectArr = CCArrayExt<MyGameObject*>(ret);
    auto stringSteamID = std::to_string(SteamUser()->GetSteamID().ConvertToUint64());
    auto editorLayer = static_cast<MyLevelEditorLayer*>(this->m_editorLayer);



    if (!gameManager->m_fields->m_isInLobby || this->m_fields->m_wasDataSent) {

        for (auto obj : objectArr) {
            auto uid = stringSteamID + "!" + std::to_string(editorLayer->m_fields->m_blocksPlaced);
            obj->m_fields->m_veryUniqueID = uid;
            editorLayer->m_fields->m_blocksPlaced += 1;
            editorLayer->m_fields->m_pUniqueIDOfGameObject->setObject(obj, obj->m_fields->m_veryUniqueID);
        }

        return ret;
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionPastedObjects)},
        {"ObjectString", p0}
    });
    
    matjson::Value eUUIDs = matjson::Value::array();


    unsigned int index = 0;
    for (auto obj : objectArr) {
        auto uid = stringSteamID + "!" + std::to_string(editorLayer->m_fields->m_blocksPlaced);
        // log::info("OBJ: {}", uid);
        eUUIDs.push(uid);
        obj->m_fields->m_veryUniqueID = uid;
        editorLayer->m_fields->m_blocksPlaced += 1;
        editorLayer->m_fields->m_pUniqueIDOfGameObject->setObject(obj, obj->m_fields->m_veryUniqueID);
        index += 1;
    }

	object["EditUUIDs"] = eUUIDs;


    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));


    return ret;
}