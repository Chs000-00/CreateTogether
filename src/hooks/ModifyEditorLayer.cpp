#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <isteamuser.h>
#include "../types/ActionTypes.hpp"
#include "ModifyGameManager.hpp"
#include "ModifyGameObject.hpp"
#include "ModifyEditorLayer.hpp"
#include "ModifyEditorUI.hpp"

using namespace geode::prelude;

bool MyLevelEditorLayer::init(GJGameLevel* p0, bool p1) {

    #ifdef USE_TEST_SERVER
    std::srand(std::time(nullptr));
    this->m_fields->m_userID = SteamUser()->GetSteamID().ConvertToUint64() + std::rand();
    #endif

    this->m_fields->m_pUniqueIDOfGameObject = CCDictionary::create();

    if (!LevelEditorLayer::init(p0, p1)) {
        return false;
    }

    // TODO: Check if you joined a lobby or not
    auto objectArr = CCArrayExt<MyGameObject*>(m_objects);

    auto stringSteamID = std::to_string(SteamUser()->GetSteamID().ConvertToUint64());

    // This might be inefficient as this requires looping over the arr twice.
    for (auto obj : objectArr) {
        auto uid = stringSteamID + std::to_string(this->m_fields->m_blocksPlaced);
        obj->m_fields->m_veryUniqueID = uid;
        this->m_fields->m_blocksPlaced += 1;
        this->m_fields->m_pUniqueIDOfGameObject->setObject(obj, obj->m_fields->m_veryUniqueID);
    }

    this->m_fields->m_loadingFinished = true;

    return true;
}


// TODO: Implement group id sync
GameObject* MyLevelEditorLayer::createObject(int p0, cocos2d::CCPoint p1, bool p2) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    MyGameObject* createdGameObject = static_cast<MyGameObject*>(LevelEditorLayer::createObject(p0, p1, p2));

    if (p2 || m_fields->m_wasDataSent) {
        return createdGameObject;
    }

    auto stringSteamID = std::to_string(this->m_fields->m_userID);
    // This string should be unique for every user
    auto uid = stringSteamID + "!" + std::to_string(this->m_fields->m_blocksPlaced);
    this->m_fields->m_blocksPlaced += 1;

    createdGameObject->m_fields->m_veryUniqueID = uid;

    this->m_fields->m_pUniqueIDOfGameObject->setObject(createdGameObject, uid);

    // Assign UUIds when a user is not in lobby
    if (!gameManager->m_fields->m_isInLobby) {
        return createdGameObject;
    }
     
    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionPlacedObject)},
        {"x", p1.x},
        {"y", p1.y},
        {"Layer", createdGameObject->m_editorLayer},
        {"ObjID", p0},
        {"ObjectUID", createdGameObject->m_fields->m_veryUniqueID}
    });

    // TODO: Find better way to do this
    // Extra values which are copied to a new object when created
    if (auto selected = this->m_editorUI->m_selectedObject) {
        if (selected->m_objectID == p0) {
            // object.set("UseExtra", true);
            object.set("Rot", selected->getRotation());
            object.set("HD", selected->m_isHighDetail);
            object.set("NoGlow", selected->m_hasNoGlow);
            object.set("NoEnter", selected->m_isDontEnter);
            object.set("NoFade", selected->m_isDontFade);
            object.set("FlipX", selected->m_isFlipX);
            object.set("FlipY", selected->m_isFlipY);
        }
    }
    
    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));
    
    return createdGameObject;
}


void MyLevelEditorLayer::updateLevelFont(int p0) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

    if (!gameManager->m_fields->m_isInLobby || m_fields->m_wasDataSent) {
        LevelEditorLayer::updateLevelFont(p0);
        return;
    }

    matjson::Value object = matjson::makeObject({
        {"Type", static_cast<int>(eActionUpdatedFont)},
        {"FontID", p0}
    });

    gameManager->sendDataToMembers(object.dump(matjson::NO_INDENTATION));

    LevelEditorLayer::updateLevelFont(p0);
}

void MyLevelEditorLayer::deleteObject(GameObject *obj) {
    // log::debug("Cleaning up.. Deleting sent object!");
    auto betterGameObject = static_cast<MyGameObject*>(obj);
    EditorUI::get()->deselectObject(obj);
    this->m_fields->m_pUniqueIDOfGameObject->removeObjectForKey(betterGameObject->m_fields->m_veryUniqueID);
    obj->deactivateObject(true);
    LevelEditorLayer::removeObjectFromSection(obj);
    this->removeSpecial(obj);
}

// TODO: Figure out args
void MyLevelEditorLayer::addToGroup(GameObject* p0, int p1, bool p2) {

    auto gameManager = static_cast<MyGameManager*>(GameManager::get());

    if (!gameManager->m_fields->m_isInLobby || this->m_fields->m_wasDataSent || !this->m_fields->m_loadingFinished) {
        LevelEditorLayer::addToGroup(p0, p1, p2);
        return;
    }

    MyGameObject* betterGameObject = static_cast<MyGameObject*>(p0);

    if (!gameManager->m_fields->m_sharedMassEdit.m_sendGroupIDEdits) {
        gameManager->m_fields->m_sharedMassEdit.m_sendGroupIDEdits = true;
        // Lets just hope you dont add and remove groups at the same time.
        gameManager->m_fields->m_sharedMassEdit.m_isAddingGroupID = true;
        // Or that we edit the same ID in one frame
        gameManager->m_fields->m_sharedMassEdit.m_groupIDToEdit = p1;
        gameManager->m_fields->m_sharedMassEdit.m_groupIDEdits = matjson::Value::array();

        gameManager->m_fields->m_sharedMassEdit.m_groupIDEdits.push(betterGameObject->m_fields->m_veryUniqueID);

    }

    // Or that EditUUIDs exists!
    gameManager->m_fields->m_sharedMassEdit.m_groupIDEdits.push(betterGameObject->m_fields->m_veryUniqueID);


    LevelEditorLayer::addToGroup(p0, p1, p2);
}


void MyLevelEditorLayer::removeFromGroup(GameObject* p0, int p1) {
      
    auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    MyGameObject* betterGameObject = static_cast<MyGameObject*>(p0);

    if  (!gameManager->m_fields->m_isInLobby || this->m_fields->m_wasDataSent || !this->m_fields->m_loadingFinished) {
        LevelEditorLayer::removeFromGroup(p0, p1);
        return;
    }

    if (!gameManager->m_fields->m_sharedMassEdit.m_sendGroupIDEdits) {
        gameManager->m_fields->m_sharedMassEdit.m_sendGroupIDEdits = true;
        // Lets just hope you dont add and remove groups at the same time.
        gameManager->m_fields->m_sharedMassEdit.m_isAddingGroupID = false;
        // Or that we edit the same ID in one frame
        gameManager->m_fields->m_sharedMassEdit.m_groupIDToEdit = p1;
        gameManager->m_fields->m_sharedMassEdit.m_groupIDEdits = matjson::Value::array();
    }

    // Or that EditUUIDs exists!
    gameManager->m_fields->m_sharedMassEdit.m_groupIDEdits.push(betterGameObject->m_fields->m_veryUniqueID);


    LevelEditorLayer::removeFromGroup(p0, p1);
}

void MyLevelEditorLayer::createFakePlayLayer() {
    log::debug("Creating a fake playlayer...");

    auto ret = PlayLayer::scene(this->m_level, false, false);
    ret->setZOrder(0);
    // ret->m_fields->m_isFakeLayer = true;
    addChild(ret);
}
