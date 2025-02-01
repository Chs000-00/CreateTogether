#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/EnhancedGameObject.hpp>
#include "../UtilMacros.hpp"
using namespace geode::prelude;

bool isValidEnumRange(int n, int min, int max);


template <typename T> 
void setJsonObjectFromProperty( T val, matjson::Value json, const char* str) {
    json.set(str, val);
}

template <typename T> 
void setJsonObjectFromProperty( T* val, matjson::Value json, const char* str) {
    json.set(str, val);
}


// TODO: Uhh this I guess
template <typename T> 
matjson::Value getJsonFromGameObject(T* obj) {
    matjson::Value ret;
    CREATE_TOGETHER_SET_JSON(m_duration);
    CREATE_TOGETHER_SET_JSON(m_opacity);
    //setJsonObjectFromProperty(obj->m_opacity, ret, "m_opacity");



    return ret;
}

template <typename T> 
matjson::Value setGameObjectFromJson(T* obj) {
    matjson::Value ret;

    return ret;
}