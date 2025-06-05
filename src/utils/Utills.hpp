#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
using namespace geode::prelude;


// Checks if a enum is within range.
// Comparison uses <=
bool isValidEnumRange(int n, int min, int max);

// Toggle stuff in levelSettings based off of an int
Result<int> toggleFromLevelSettings(LevelSettingsObject* settings, int option);