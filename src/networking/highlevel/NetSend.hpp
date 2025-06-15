#pragma once
#include "SharedHighLevelHeaders.hpp"

// Too lazy to write out std::vector<flatbuffers::Offset<flatbuffers::String>>
typedef std::vector<flatbuffers::Offset<flatbuffers::String>> IDList;


void addStringToIDList(IDList& uniqueIDList, const char* str);

void sendCreateObjects(const char* uniqueID, CCPoint pos, float rotation, bool isHighDetail, bool noGlow, bool noEnter, bool flipX, bool flipY);

// Ref is prob better so we don't have to copy the vec each time.
void sendMoveObjects(IDList& uniqueIDList, CCPoint offset);

void sendRotateObjects(IDList& uniqueIDList, float rotation, CCPoint anchor);

void sendDeleteObjects(IDList& uniqueIDList);