#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/EnhancedGameObject.hpp>
#include "../UtilMacros.hpp"
using namespace geode::prelude;

// Checks if a enum is within range.
// Comparison uses <=
bool isValidEnumRange(int n, int min, int max);