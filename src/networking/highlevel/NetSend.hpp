#pragma once
#include "SharedHighLevelHeaders.hpp"

// Too lazy to write out std::vector<flatbuffers::Offset<flatbuffers::String>>
typedef std::vector<flatbuffers::Offset<flatbuffers::String>> IDList;


void addStringToIDList(IDList& uniqueIDList, const char* str);

void sendCreateObjects(const char* uniqueID, uint64_t objectID, CCPoint pos, float rotation, bool isHighDetail, bool noGlow, bool noEnter, bool flipX, bool flipY);

void sendMoveObjects(const char* uniqueID, CCPoint offset);

void sendRotateObjects(IDList& uniqueIDList, float rotation, CCPoint anchor);

void sendDeleteObjects(IDList& uniqueIDList);

void sendPasteObjects(IDList& uniqueIDList, const char* copyStr);

void sendUpdateSong(uint64_t songID);