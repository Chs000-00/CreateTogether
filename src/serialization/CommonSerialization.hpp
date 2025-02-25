#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;


// Cerealize CCPoint to json
template <>
struct matjson::Serialize<cocos2d::CCPoint> {
    static geode::Result<cocos2d::CCPoint> fromJson(const matjson::Value& value) {

        GEODE_UNWRAP_INTO(double x, data["x"].asDouble());
        GEODE_UNWRAP_INTO(double y, data["y"].asDouble());

        cocos2d::CCPoint pos = {static_cast<float>(x), static_cast<float>(y)};
    

        return geode::Ok(pos);
    }
    static matjson::Value toJson(const cocos2d::CCPoint& value) {
        return matjson::makeObject({
            { "x", value.x },
            { "y", value.y }
        });
    }
};