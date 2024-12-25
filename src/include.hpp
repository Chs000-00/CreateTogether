#pragma once

#include "config.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <isteammatchmaking.h>
#include <isteamnetworkingmessages.h>
#include <isteamuser.h>
#include <uuid_v4.h>
#include <isteammatchmaking.h>
#include "types/ActionTypes.hpp"
#include "types/LobbyData.hpp"
#include "layers/LobbiesLayer.hpp"
#include "hooks/ModifyEditorUI.hpp"
#include "hooks/ModifyEditorLayer.hpp"
#include "hooks/ModifyGameObject.hpp"
#include "hooks/ModifyGameManager.hpp"
#include "ui/LevelListBorders.hpp"
#include "ui/LobbyPopup.hpp"
#include "UtilMacros.hpp"
// Once again, only include the libs for sockets if you are using the test server!
#ifdef USE_TEST_SERVER
    #define WIN32_LEAN_AND_MEAN
    #include "types/PlaceboMsg.hpp"
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
#endif

using namespace geode::prelude;