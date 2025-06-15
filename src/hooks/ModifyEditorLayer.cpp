#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <isteamuser.h>
#include "../types/ActionTypes.hpp"
#include "ModifyGameManager.hpp"
#include "../networking/HighLevelHeader.hpp"
#include "../networking/NetManager.hpp"
#include "ModifyGameObject.hpp"
#include "ModifyEditorLayer.hpp"


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

    return LevelEditorLayer::createObject(p0, p1, p2);

    // auto gameManager = static_cast<MyGameManager*>(GameManager::get());
    // MyGameObject* createdGameObject = static_cast<MyGameObject*>(LevelEditorLayer::createObject(p0, p1, p2));

    // if (p2 || m_fields->m_wasDataSent) {
    //     return createdGameObject;
    // }

    // auto stringSteamID = std::to_string(this->m_fields->m_userID);
    // // This string should be unique for every user
    // auto uid = stringSteamID + "!" + std::to_string(this->m_fields->m_blocksPlaced);
    // this->m_fields->m_blocksPlaced += 1;

    // createdGameObject->m_fields->m_veryUniqueID = uid;

    // this->m_fields->m_pUniqueIDOfGameObject->setObject(createdGameObject, uid);

    // // WHAT THE HELL IS THIS????? TODO: CHANGE THIS!!
    // auto selected = this->m_editorUI->m_selectedObject;

    // // Assign UUIds when a user is not in lobby
    // if (!NetManager::getIsInLobby()) {
    //     return createdGameObject;
    // }

    // sendCreateObjects(uid.c_str(), p1, selected->getRotation(), selected->m_isHighDetail, selected->m_hasNoGlow, selected->m_isDontEnter, selected->m_isFlipX, selected->m_isFlipY);
     
    // return createdGameObject;
}