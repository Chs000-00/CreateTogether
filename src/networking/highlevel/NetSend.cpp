#include "../HighLevelHeader.hpp"
#include "SharedHighLevelHeaders.hpp"

void addStringToIDList(IDList& uniqueIDList, const char* str) {
    auto netManager = NetManager::get();
    uniqueIDList.push_back(netManager->m_builder.CreateString(str));
}

void sendCreateObjects(const char* uniqueID, uint64_t objectID, CCPoint pos, float rotation, bool isHighDetail, bool noGlow, bool noEnter, bool flipX, bool flipY) {
    auto netManager = NetManager::get();
    auto objectPos = CTSerialize::CCPosI(pos.x, pos.y);
    auto minObj = CTSerialize::CreateGDGameObjectMinDirect(netManager->m_builder, uniqueID, objectID, &objectPos, rotation, isHighDetail, noGlow, noEnter); //uhh add stuff here
	auto createObjectsOffset = CTSerialize::CreateCreateObjects(netManager->m_builder, minObj);
    auto messageHeaderOffset = CTSerialize::CreateMessageHeader(netManager->m_builder, CTSerialize::MessageBody_CreateObjects, createObjectsOffset.Union());
	netManager->sendMessage(messageHeaderOffset);
	netManager->m_builder.Clear();

}

void sendMoveObjects(const char* uniqueID, CCPoint offset) {
    auto netManager = NetManager::get();
    auto offsetPos = CTSerialize::CCPos(offset.x, offset.y);
    auto moveObjectsOffset = CTSerialize::CreateMoveObjectsDirect(netManager->m_builder, uniqueID, &offsetPos);
    auto messageHeaderOffset = CTSerialize::CreateMessageHeader(netManager->m_builder, CTSerialize::MessageBody_MoveObjects, moveObjectsOffset.Union());
	netManager->sendMessage(messageHeaderOffset);
	netManager->m_builder.Clear();
}

void sendRotateObjects(IDList& uniqueIDList, float rotation, CCPoint anchor) {
    auto netManager = NetManager::get();
    auto objectPos = CTSerialize::CCPos(anchor.x, anchor.y);
    auto rotateObjectsOffset = CTSerialize::CreateRotateObject(netManager->m_builder, rotation, &objectPos, netManager->m_builder.CreateVector(uniqueIDList));
    auto messageHeaderOffset = CTSerialize::CreateMessageHeader(netManager->m_builder, CTSerialize::MessageBody_RotateObject, rotateObjectsOffset.Union());
	netManager->sendMessage(messageHeaderOffset);
	netManager->m_builder.Clear();
}

void sendDeleteObjects(IDList& uniqueIDList) {
    auto netManager = NetManager::get();
    auto deleteObjectsMessage = CTSerialize::CreateDeleteObjects(netManager->m_builder, netManager->m_builder.CreateVector(uniqueIDList));
    auto messageHeaderOffset = CTSerialize::CreateMessageHeader(netManager->m_builder, CTSerialize::MessageBody_DeleteObjects, deleteObjectsMessage.Union());
	netManager->sendMessage(messageHeaderOffset);
	netManager->m_builder.Clear();
}

void sendPasteObjects(IDList& uniqueIDList, const char* copyStr) {
    auto netManager = NetManager::get();
    auto pasteObjectsMessage = CTSerialize::CreatePasteObjects(netManager->m_builder, netManager->m_builder.CreateVector(uniqueIDList), netManager->m_builder.CreateString(copyStr));
    auto messageHeaderOffset = CTSerialize::CreateMessageHeader(netManager->m_builder, CTSerialize::MessageBody_PasteObjects, pasteObjectsMessage.Union());
	netManager->sendMessage(messageHeaderOffset);
	netManager->m_builder.Clear();
}

void sendUpdateSong(uint64_t songID) {
    auto netManager = NetManager::get();
    auto updateSongMessage = CTSerialize::CreateUpdateSong(netManager->m_builder, songID);
    auto messageHeaderOffset = CTSerialize::CreateMessageHeader(netManager->m_builder, CTSerialize::MessageBody_UpdateSong, updateSongMessage.Union());
	netManager->sendMessage(messageHeaderOffset);
	netManager->m_builder.Clear();
}