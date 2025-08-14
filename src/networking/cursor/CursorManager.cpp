#include "CursorManager.hpp"
#include "NetManager.hpp"
#include "ccserialization_generated.h"
#include "ctcursor_generated.h"
#include "../../hooks/ModifyEditorLayer.hpp"


void CursorManager::sendCursorUpdateToAll() {
    auto level = static_cast<MyLevelEditorLayer*>(LevelEditorLayer::get());

    if (!level) {
        return;
    }

    auto pos = CTSerialize::CCPos();
    CTSerialize::cursor::CreateCursorUpdate(this->m_cursorBuilder, &pos, CTSerialize::cursor::StatusType_None);
}

CursorManager* CursorManager::get() {
    return NetManager::get()->m_cursorManager;
}