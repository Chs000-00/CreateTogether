#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>

#ifdef NO_STEAMWORKS
    #include <debug/steamnetworkingtypes.h>
#else
    #include <steamnetworkingtypes.h>
#endif

using namespace geode::prelude;

#define SERIALIZE_AND_RECEIVE(msgType, ...) auto GEODE_CONCAT(sarmacro, __LINE__) = recv##msgType (static_cast<const CTSerialize::msgType *>(dmsg), ##__VA_ARGS__); \
 if (GEODE_CONCAT(sarmacro, __LINE__).isErr()) return GEODE_CONCAT(sarmacro, __LINE__)

// Checks if a enum is within range.
// Comparison uses <=
bool isWithinRange(int n, int min, int max);

// Toggle stuff in levelSettings based off of an int
Result<uint8_t> toggleFromLevelSettings(LevelSettingsObject* settings, int option);

// Change the SteamNetworkingIdentity to a string for hashing, so the cursor code could work properly.
std::string getCursorHash(SteamNetworkingIdentity id);

// New function to get a hash from a vuid. Still needs to be worked on.
//std::string getvUIDHash(SteamNetworkingIdentity id);