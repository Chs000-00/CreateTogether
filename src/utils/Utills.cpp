#include "Utills.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

// Checks if a enum is within range.
// Comparison uses <=
bool isValidEnumRange(int n, int min, int max) {
    return (min <= n && n <= max);
}

// I dont think void* would work here
template <typename T>
Result<int> unwrapToData(void* propertyRef) {

}

// Todo: This
matjson::Value getJsonFromEffectGameObject(EffectGameObject*) {

}

Result<EffectGameObject*> getEffectGameObjectFromMatjson(matjson::Value) {

}

// Todo: This
matjson::Value getJsonFromEnhancedGameObject(EnhancedGameObject*) {

}

Result<EnhancedGameObject*> getEnhancedGameObjectFromMatjson(matjson::Value) {

}