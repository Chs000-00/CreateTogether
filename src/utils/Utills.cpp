#include "Utills.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

// Checks if a enum is within range.
// Comparison uses <=
bool isValidEnumRange(int n, int min, int max) {
    return (min <= n && n <= max);
}

// // I dont think void* would work here
// // Nor ##T
//
// template <typename T>
// Result<int> unwrapToData(void* propertyRef, matjson::Value json, const char* data) {
//     GEODE_UNWRAP_INTO(T res, json[data].as ##T ());
//     propertyRef = res;
//     return Ok(0);
// }

// Todo: This
// Returns an effectGameObject. Includes EnchancedGameObject.
matjson::Value getJsonFromEffectGameObject(EffectGameObject* obj) {

}

Result<EffectGameObject*> getEffectGameObjectFromMatjson(matjson::Value) {

}

// Todo: This
matjson::Value getJsonFromEnhancedGameObject(EnhancedGameObject* obj) {
    matjson::Value ret;
    ret.set("Duration", obj->m_duration)
}

Result<EnhancedGameObject*> getEnhancedGameObjectFromMatjson(matjson::Value) {

}