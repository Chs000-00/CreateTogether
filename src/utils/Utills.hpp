#pragma once

#include "debug/steamnetworkingtypes.h"
#include <Geode/Geode.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
#include <string>
using namespace geode::prelude;

#define SERIALIZE_AND_RECEIVE(msgType, ...) auto GEODE_CONCAT(sarmacro, __LINE__) = recv##msgType (static_cast<const CTSerialize::msgType *>(dmsg), ##__VA_ARGS__); \
 if (GEODE_CONCAT(sarmacro, __LINE__).isErr()) return GEODE_CONCAT(sarmacro, __LINE__)

// Checks if a enum is within range.
// Comparison uses <=
bool isValidEnumRange(int n, int min, int max);

// Toggle stuff in levelSettings based off of an int
Result<uint8_t> toggleFromLevelSettings(LevelSettingsObject* settings, int option);

std::string getCursorHash(SteamNetworkingIdentity id);