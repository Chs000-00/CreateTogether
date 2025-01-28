#include "Utills.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

// Checks if a enum is within range.
// Comparison uses <=
bool isValidEnumRange(int n, int min, int max) {
    return (min <= n && n <= max);
}

// Todo: This
matjson::Value getJsonFromEffectGameObject(EffectGameObject*) {

}

EffectGameObject* getEffectGameObjectFromMatjson(matjson::Value) {

}

// Todo: This
matjson::Value getJsonFromEnhancedGameObject(EnhancedGameObject*) {

}

EnhancedGameObject* getEnhancedGameObjectFromMatjson(matjson::Value) {

}