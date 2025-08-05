#pragma once
#include "SharedHighLevelHeaders.hpp"

// Too lazy to write out std::vector<flatbuffers::Offset<flatbuffers::String>>
typedef std::vector<flatbuffers::Offset<flatbuffers::String>> IDList;


void addStringToIDList(IDList& uniqueIDList, const char* str);

void sendCreateObjects(const char* uniqueID, uint64_t objectID, CCPoint pos, float rotation, bool isHighDetail, bool noGlow, bool noEnter, bool flipX, bool flipY, float scaleX, float scaleY);

void sendDeleteObjects(IDList& uniqueIDList);

void sendMoveObjects(const char* uniqueID, CCPoint offset);

void sendLevelSettingChange(uint16 settingID);

void sendSpawnGroupChange(int spawngroup);

void sendRotateObjects(IDList& uniqueIDList, float rotation, CCPoint anchor);

void sendPasteObjects(IDList& uniqueIDList, const char* copyStr);

void sendModifyObjects(IDList& uniqueIDList, const char* copyStr);

// TODO: we might still be able to send copyColorID, but I need to research this more
void sendChangeDefaultColor(int groupID, ccColor3B currentColor, ccHSVValue hsv, int blending, float opacity, bool copyOpacity, int copyColorID);

void sendUpdateSong(uint64_t songID);

void sendGameModeChange(uint8 gameMode);