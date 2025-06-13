#include "../HighLevelHeader.hpp"
#include "SharedHighLevelHeaders.hpp"

void sendCreateObjects(const char* uniqueID, CCPoint pos, float rotation, bool isHighDetail, bool noGlow, bool noEnter, bool flipX, bool flipY) {

    auto objectPos = CTSerialize::CCPosI(pos.x, pos.y);

    auto minObj = CTSerialize::CreateGDGameObjectMinDirect(NetManager::get()->m_builder, uniqueID, &objectPos, rotation, isHighDetail, noGlow, noEnter); //uhh add stuff here
    
}

void sendMoveObjects(std::vector<const char*>& uniqueIDList, CCPoint offset) {

}

void sendRotateObjects(std::vector<const char*>& uniqueIDList, float rotation, CCPoint anchor) {

}

void sendDeleteObjects(std::vector<const char*>& uniqueIDList) {
    std::vector<flatbuffers::Offset<flatbuffers::String>> uniqueIDStringListInternal;

    auto netManager = NetManager::get();


    // TS IS GOING TO BE MORE INEFFICIENT THEN MATJSON 🔥🔥🔥
    for (auto id : uniqueIDList) {
        flatbuffers::Offset<flatbuffers::String> sID = netManager->m_builder.CreateString(id);
        uniqueIDStringListInternal.push_back(sID);
    }

    auto deleteObjectsMessage = CTSerialize::CreateDeleteObjects(netManager->m_builder, netManager->m_builder.CreateVector(uniqueIDStringListInternal));
}