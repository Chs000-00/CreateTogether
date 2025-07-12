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

void sendMoveObjects(IDList& uniqueIDList, CCPoint offset) {
    auto netManager = NetManager::get();
    auto createMobeObjectsOffset = CTSerialize::CreateMoveObjects(netManager->m_builder, netManager->m_builder.CreateVector(uniqueIDList));
}

void sendRotateObjects(IDList& uniqueIDList, float rotation, CCPoint anchor) {

}

void sendDeleteObjects(IDList& uniqueIDList) {
    std::vector<flatbuffers::Offset<flatbuffers::String>> uniqueIDStringListInternal;
    auto netManager = NetManager::get();
    auto deleteObjectsMessage = CTSerialize::CreateDeleteObjects(netManager->m_builder, netManager->m_builder.CreateVector(uniqueIDList));
}