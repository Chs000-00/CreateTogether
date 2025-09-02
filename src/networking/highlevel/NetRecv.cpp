#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "HighLevelHeader.hpp"
#include "NetSend.hpp"
#include "SharedHighLevelHeaders.hpp"
#include "../../hooks/ModifyEditorLayer.hpp"
#include "../../hooks/ModifyGameObject.hpp"
#include "../../utils/Utills.hpp"
#include "../../types/Cursor.hpp"
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

    if (auto scale = minObj->scale()) {
        placedGameObject->setScaleX(scale->x());
        placedGameObject->setScaleY(scale->y());
    }

    MyGameObject* betterPlacedGameObject = static_cast<MyGameObject*>(placedGameObject);
    betterPlacedGameObject->m_fields->m_veryUniqueID = minObj->uniqueID()->str();
    level->m_fields->m_pUniqueIDOfGameObject->setObject(placedGameObject, minObj->uniqueID()->str());
    return Ok(0);
}

// TODO: Test what happens if user has an object open which was deleted!!
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

Result<uint8_t> recvLevelSettingChange(const CTSerialize::LevelSettingChange* msg) {
    auto settingID = msg->settingID();
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());

    if (settingID == 13) {

        if (!msg->spawnGroup()) { 
            return Err("recvLevelSettingChange: No value for spawngroup"); 
        }

        level->m_levelSettings->m_spawnGroup = msg->spawnGroup().value();
    }

    return toggleFromLevelSettings(level->m_levelSettings, settingID);
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

// TODO: Test what happens if user has an object open which was deleted!!
Result<uint8_t> recvModifyObjects(const CTSerialize::ModifyObjects* msg) {
    auto pasteString = msg->pastedString();
    auto uniqueIDList = msg->uniqueIDList();
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());

    // TODO: Figure out args
    auto objectArr = CCArrayExt<MyGameObject*>(level->createObjectsFromString(pasteString->str(), false, false));

    if (objectArr.size() != uniqueIDList->size()) {
        log::warn("recvModifyObjects: Mismatched objectArr and editUUIDs size!");
    }

    for (auto i = 0; i != std::min(objectArr.size(), (size_t)uniqueIDList->size()); ++i) {
        auto newObj = (objectArr[i]);
        auto uid = uniqueIDList->Get(i)->c_str();

        if (auto oldObj = static_cast<MyGameObject*>(level->m_fields->m_pUniqueIDOfGameObject->objectForKey(uid))) {
            level->deleteObject(oldObj);
            newObj->m_fields->m_veryUniqueID = uid;
            level->m_fields->m_pUniqueIDOfGameObject->setObject(newObj, uid);
        }
        else {
            return Err("object UID not found");
        }
    }
    return Ok(0);
}

Result<uint8_t> recvChangeDefaultColor(const CTSerialize::ChangeDefaultColor* msg) {
    
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());

    if (auto color = msg->currentColor()) {
        auto newColor = ccColor3B(color->r(), color->g(), color->b());

        // getColorAction automatically checks the ranges for the ID's
        auto colorAction = level->m_levelSettings->m_effectManager->getColorAction(msg->colorID());

        colorAction->m_fromColor = newColor;
        colorAction->m_blending = msg->blending();
        colorAction->m_toOpacity = msg->opacity();

        // colorID:int;
        // currentColor:CCColor3B;
        // hsv:CCHsvValue;
        // blending:bool;
        // opacity:float;
        // copyOpacity:bool;
        // copyColorID:int = null;

    }
    else {
        return Err("recvChangeDefaultColor: no color found");
    }

    return Ok(0);
}

Result<uint8_t> recvRequestLevel(const CTSerialize::RequestLevel* msg, SteamNetworkingIdentity msgSource) {
    auto netManager = NetManager::get();

    if (netManager->m_isHost) {
        sendReturnLevelString();
        return Ok(1);
    }

    return Ok(0);
}

// TODO: Enter the level editor only when this is called, so we can properly load the level.
Result<uint8_t> recvReturnLevelString(const CTSerialize::ReturnLevelString* msg, SteamNetworkingIdentity msgSource) {
    auto netManager = NetManager::get();

    if (!netManager->m_isRequestingLevelString) {
        return Err("recvReturnLevelString: Not requesting a level string.");
    }

    #ifdef NO_STEAMWORKS

    if (netManager->m_hostID != msgSource.GetSteamID()) {
        return Err("eActionReturnLevelString: Non-Host level string");
    }

    if (msg->levelString()->str() == "ignore") {
        log::info("Ignored");
        auto gameLevel = GJGameLevel::create();
        gameLevel->m_isEditable = true;
        gameLevel->m_levelType = GJLevelType::Editor;
        // gameLevel->m_levelDesc += "Created with Create Together";
        auto lev = LevelEditorLayer::create(gameLevel, false);

        switchToScene(lev);
        netManager->m_wasDataSent = false;
        netManager->m_isRequestingLevelString = false;

        log::info("IsInLobby {}", netManager->m_isInLobby);

        return Ok(1);
    }

    #endif

    auto uniqueIDList = msg->uniqueIDList();

    std::string baller = ZipUtils::compressString(msg->levelString()->str(), false, 0);

    auto gameLevel = GJGameLevel::create();
    gameLevel->m_levelType = GJLevelType::Editor;
    gameLevel->m_levelString = baller;
    auto lev = LevelEditorLayer::create(gameLevel, false);

    auto objectArr = CCArrayExt<MyGameObject*>(lev->m_objects);

    switchToScene(lev);
    
    auto betterLevel = static_cast<MyLevelEditorLayer*>(lev);


    netManager->m_wasDataSent = false;

    if (!betterLevel) {
        return Err("recvReturnLevelString: lev is nullptr");
    }

    for (auto i = 0; i != std::min(objectArr.size(), (size_t)uniqueIDList->size()); ++i) {
        auto mObject = (objectArr[i]);
        auto uid = uniqueIDList->Get(i)->str();
        mObject->m_fields->m_veryUniqueID = uid;
        betterLevel->m_fields->m_pUniqueIDOfGameObject->setObject(mObject, uid);
    }

	netManager->m_isRequestingLevelString = false;

    sendRequestForCursors();

    return Ok(0);
}

Result<uint8_t> recvUpdateSong(const CTSerialize::UpdateSong* msg) {
    auto songID = msg->songID();
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());
    level->m_level->m_songID = songID;
    level->levelSettingsUpdated();
    return Ok(0);
}

// TODO: Check possible range errors
// TODO: Middleground synchronization
Result<uint8_t> recvChangeArt(const CTSerialize::ChangeArt* msg) {
    auto artType = msg->artType();
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());

    switch (artType) {
        case CTSerialize::ArtType_Ground:
            level->createGroundLayer(msg->artID(), msg->line());
            break;

        case CTSerialize::ArtType_Background:
            level->createBackground(msg->artID());
            break;
        
        default:
            return Err("recvChangeArt: missing artType case");
            break;
    }


    return Ok(0);
}

Result<uint8_t> recvSpeedChange(const CTSerialize::SpeedChange* msg) {
    auto speed = msg->speed();
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());

    if (!isValidEnumRange(speed, CTSerialize::Speed_MIN, CTSerialize::Speed_MAX)) {
        return Err("recvSpeedChange: invalid speed");
    }

    level->m_levelSettings->m_startSpeed = static_cast<Speed>(speed);

    return Ok(0);
}

// TODO: Check issues that may occur when using invalid ranges
Result<uint8_t> recvGameModeChange(const CTSerialize::GameModeChange* msg) {
    auto gameMode = msg->gameMode();
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());
    level->m_levelSettings->m_startMode = gameMode;
    level->levelSettingsUpdated();
    return Ok(0);
}

Result<uint8_t> recvPlayerCursorData(const CTSerialize::PlayerCursorData* msg, SteamNetworkingIdentity msgSource) {

    auto cursorManager = CursorManager::get();
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());


    auto cursorHash = getCursorHash(msgSource);

    CCNode* cursorLayer = level->m_objectLayer->getChildByID("cursor-layer"_spr);

    if (!cursorLayer) {
        log::info("Creating cursor-layer.");
        cursorLayer = CCLayer::create();
        cursorLayer->setID("cursor-layer"_spr);
        level->addChild(cursorLayer);
    }    

    CreateTogetherCursor* cursor;
    CreateTogetherCursor::CursorData data;

    
    if (auto wave = msg->playerWave()) {

        data = CreateTogetherCursor::CursorData(
            wave->cursorColor1(),
            wave->cursorColor2(),
            wave->cursorID(),
            wave->cursorHasGlow(),
            wave->cursorGlowColor()
        );
    } 
    else {
        data = CreateTogetherCursor::defaultCursorData(2, 3);
        
    }

    if (cursorManager->m_playerCursors.contains(cursorHash)) {
        cursorManager->m_playerCursors.at(cursorHash)->updateCursor(data);
    }

    else {
        cursor = CreateTogetherCursor::create(data);

        // TODO: release this
        cursor->retain();

        cursorLayer->addChild(cursor);

        cursorManager->m_playerCursors.insert({cursorHash, cursor});
    }

    if (!msg->playerWave()) {
        return Err("recvRequestLevel: message has no wave object. Using default wave.");
    }

    return Ok(0);
}

Result<uint8_t> recvRequestForCursors(const CTSerialize::RequestForCursors* msg) {
    sendPlayerCursorData();
    return Ok(0);
}