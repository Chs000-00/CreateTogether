#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/EnhancedGameObject.hpp>
#include "../UtilMacros.hpp"
using namespace geode::prelude;

bool isValidEnumRange(int n, int min, int max);

// TODO: Place this stuff in another file
template < typename T > 
void setJsonObjectFromProperty( T val, matjson::Value json, const char* str) {
    json.set(str, val);
}

template < typename T > 
void setJsonObjectFromProperty( T* val, matjson::Value json, const char* str) {
    json.set(str, val);
}


template < typename T > 
matjson::Value getJsonFromGameObject(T* obj) {

    static_assert(std::is_base_of<GameObject, T>::value,"T must be a derived class of GameObject in getJsonFromGameObject<T>.");

    matjson::Value ret;

    if constexpr (std::is_base_of<EffectGameObject, T>::value) {
        CREATE_TOGETHER_SET_JSON(m_duration);
        CREATE_TOGETHER_SET_JSON(m_opacity);
        CREATE_TOGETHER_SET_JSON(m_targetGroupID);
        CREATE_TOGETHER_SET_JSON(m_centerGroupID);
        CREATE_TOGETHER_SET_JSON(m_isTouchTriggered);
        CREATE_TOGETHER_SET_JSON(m_isSpawnTriggered);
        CREATE_TOGETHER_SET_JSON(m_hasCenterEffect);
        CREATE_TOGETHER_SET_JSON(m_shakeStrength);
        CREATE_TOGETHER_SET_JSON(m_shakeInterval);
        CREATE_TOGETHER_SET_JSON(m_tintGround);
        CREATE_TOGETHER_SET_JSON(m_usesPlayerColor1);
        CREATE_TOGETHER_SET_JSON(m_usesPlayerColor2);

    }


    return ret;
}

// TODO: Uhh this I guess
template < typename T > 
matjson::Value setGameObjectFromJson(T* obj) {
    matjson::Value ret;

    return ret;
}