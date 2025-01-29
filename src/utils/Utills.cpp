#include "Utills.hpp"

#include <Geode/Geode.hpp>
#include "../UtilMacros.hpp"
using namespace geode::prelude;



// Checks if a enum is within range.
// Comparison uses <=
bool isValidEnumRange(int n, int min, int max) {
    return (min <= n && n <= max);
}

template <typename T> 
inline void setJsonObjectFromProperty(matjson::Value json, T* val, const char* str) {
    json.set(str, val)
}

template <typename T> 
void getJsonFromGameObject(T* obj) {
    matjson::Value ret;
    CREATE_TOGETHER_SET_JSON(m_duration);
}

// Todo: This
// Returns an effectGameObject. Includes EnchancedGameObject.
// matjson::Value getJsonFromEffectGameObject(EffectGameObject* obj) {

// }

// Result<EffectGameObject*> getEffectGameObjectFromMatjson(matjson::Value) {

// }

// // Todo: This
// matjson::Value getJsonFromEnhancedGameObject(EnhancedGameObject* obj) {
//     matjson::Value ret;
//     ret.set("Duration", obj->m_duration)
//     ret.set("Opacity", obj->m_opacity)
//     ret.set("TargetGroupID", obj->m_targetGroupID)
//     ret.set("CenterGroupId", obj->m_centerGroupId)
//     ret.set("IsTouchTriggered", obj->m_isTouchTriggered)

// }

// Result<EnhancedGameObject*> getEnhancedGameObjectFromMatjson(matjson::Value) {

// }