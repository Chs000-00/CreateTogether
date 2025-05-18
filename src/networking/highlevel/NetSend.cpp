#include "../HighLevelHeader.hpp"
#include "SharedHighLevelHeaders.hpp"

void sendCreateObjects(const char* uniqueID, CCPoint pos, float rotation, bool isHighDetail, bool noGlow, bool noEnter, bool flipX, bool flipY) {
    flatbuffers::FlatBufferBuilder builder(512);

    auto objectPos = CTSerialize::CCPosI(pos.x, pos.y);

    auto minObj = CTSerialize::CreateGDGameObjectMinDirect(builder, uniqueID, &objectPos, rotation, isHighDetail, noGlow, noEnter); //uhh add stuff here

}

void sendMoveObjects(std::vector<const char*> uniqueIDList, CCPoint offset) {

}

void sendRotateObjects(std::vector<const char*> uniqueIDList, float rotation, CCPoint anchor) {

}