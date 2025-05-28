#pragma once
#include "SharedHighLevelHeaders.hpp"

void sendCreateObjects(const char* uniqueID, CCPoint pos, float rotation, bool isHighDetail, bool noGlow, bool noEnter, bool flipX, bool flipY);

void sendMoveObjects(std::vector<const char*> uniqueIDList, CCPoint offset);

void sendRotateObjects(std::vector<const char*> uniqueIDList, float rotation, CCPoint anchor);