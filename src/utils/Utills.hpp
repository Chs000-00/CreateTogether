#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
using namespace geode::prelude;

// TODO: Test this
// Unwrap a buffer into a ok() or err()
#define UNWRAP_BUFFER(outvar, ...)                                             \
    auto res##__LINE__ = __VA_ARGS__;                                          \
    if (res##__LINE__)                                                         \
        return geode::Err(std::move(res##__LINE__).unwrapErr());               \
    outvar = std::move(res##__LINE__)

// Checks if a enum is within range.
// Comparison uses <=
bool isValidEnumRange(int n, int min, int max);

// Toggle stuff in levelSettings based off of an int
Result<int> toggleFromLevelSettings(LevelSettingsObject* settings, int option);