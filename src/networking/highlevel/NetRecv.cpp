#include "HighLevelHeader.hpp"
#include "SharedHighLevelHeaders.hpp"
#include "../../hooks/ModifyEditorLayer.hpp"
#include "../../hooks/ModifyGameObject.hpp"
#include <Geode/binding/EditorUI.hpp>



// TODO: Verifier stuff
// TODO: Fix long slopes offset being weird
Result<uint8_t> recvCreateObjects(const CTSerialize::CreateObjects* msg) {
    auto minObj = msg->obj();
    cocos2d::CCPoint gameObjectPos = {static_cast<float>(minObj->pos()->x()), static_cast<float>(minObj->pos()->y())};

    // TODO: nullptr check this
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());
    // TODO: Figure if a race condition is possible
    NetManager::get()->m_wasDataSent = true;
    GameObject* placedGameObject = level->createObject(minObj->objID(), gameObjectPos, false);
    NetManager::get()->m_wasDataSent = false;

    placedGameObject->setRotation(minObj->rotation());
    placedGameObject->m_isHighDetail = minObj->isHighDetail();
    placedGameObject->m_hasNoGlow = minObj->noGlow();
    placedGameObject->m_isDontEnter = minObj->noEnter();

    if(auto flip = minObj->flip()) {
        placedGameObject->setFlipX(flip->flipX());
        placedGameObject->setFlipY(flip->flipY());
    }

    MyGameObject* betterPlacedGameObject = static_cast<MyGameObject*>(placedGameObject);
    betterPlacedGameObject->m_fields->m_veryUniqueID = minObj->uniqueID()->str();
    level->m_fields->m_pUniqueIDOfGameObject->setObject(placedGameObject, minObj->uniqueID()->str());

    return Ok(0);
}

Result<uint8_t> recvDeleteObjects(const CTSerialize::DeleteObjects* msg) {
    auto idlist = msg->uniqueIDList();

    auto editor = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());
    for (auto i = 0; i < idlist->Length(); i++) {
        if (auto dObj = static_cast<GameObject*>(editor->m_fields->m_pUniqueIDOfGameObject->objectForKey(idlist->Get(i)->str()))) {
            editor->deleteObject(dObj);
        }
    }
    return Ok(0);
}

Result<uint8_t> recvMoveObjects(const CTSerialize::MoveObjects* msg) {
    auto id = msg->uniqueID();
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());
    cocos2d::CCPoint offsetPos = {msg->positionOffset()->x(), msg->positionOffset()->y()};


    if (auto dObj = level->m_fields->m_pUniqueIDOfGameObject->objectForKey(id->str())) {
        NetManager::get()->m_wasDataSent = true;
        level->m_editorUI->moveObject(static_cast<GameObject*>(dObj), offsetPos);
        NetManager::get()->m_wasDataSent = false;
    }
    return Ok(0);
}

Result<uint8_t> recvRotateObjects(const CTSerialize::RotateObjects* msg) {
    auto rot = msg->rotation();
    cocos2d::CCPoint pos = {msg->anchor()->x(), msg->anchor()->y()};


    auto idlist = msg->uniqueIDList();

    auto editor = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());
    

    auto arr = CCArray::create();

    for (auto i = 0; i < idlist->Length(); i++) {
        if (auto dObj = static_cast<GameObject*>(editor->m_fields->m_pUniqueIDOfGameObject->objectForKey(idlist->Get(i)->str()))) {
            arr->addObject(dObj);
            // dObj->setRotation(rot + dObj->getRotation());
        }
    }

    NetManager::get()->m_wasDataSent = true;
    editor->m_editorUI->rotateObjects(arr, rot, pos);
    NetManager::get()->m_wasDataSent = false;


    return Ok(0);
}

Result<uint8_t> recvPasteObjects(const CTSerialize::PasteObjects* msg) {
    auto pasteString = msg->pastedString();
    auto uniqueIDList = msg->uniqueIDList();
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());

    // TODO: Figure out args
    auto objectArr = CCArrayExt<MyGameObject*>(level->createObjectsFromString(pasteString->str(), false, false));

    // This might be inefficient as this requires looping over the arr twice.
    for (auto i = 0; i != std::min(objectArr.size(), (size_t)uniqueIDList->size()); ++i) {
        auto mObject = (objectArr[i]);
        auto uid = uniqueIDList->Get(i)->str();
        mObject->m_fields->m_veryUniqueID = uid;
        level->m_fields->m_pUniqueIDOfGameObject->setObject(mObject, uid);
    }
    return Ok(0);
}

Result<uint8_t> recvUpdateSong(const CTSerialize::UpdateSong* msg) {
    auto songID = msg->songID();
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());
    level->m_level->m_songID = songID;
    level->levelSettingsUpdated();
    return Ok(0);
}