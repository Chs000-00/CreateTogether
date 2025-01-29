#pragma once

#include "config.hpp"


// TODO: STOP USING MACROS!
// TODO: I DONT NEED THESE MACROS
#define GET_OBJECT_FROM_UID static_cast<GameObject*>(level->m_fields->m_pUniqueIDOfGameObject->objectForKey(data["ObjectUID"].asString().ok().value()))
#define GET_CCPOINT static_cast<float>(unwrappedMessage["x"].asInt().ok().value()), static_cast<float>(unwrappedMessage["y"].asInt().ok().value())

#define CREATE_TOGETHER_SET_JSON(prop) setJsonObjectFromProperty(obj->##prop , #prop)

// TODO: Finish this
#define CREATE_TOGETHER_JSON_TO_OBJ(prop) setJsonObjectFromProperty(obj->##prop , #prop)

// log::error("Message Invalidated at line {} and key {}", __LINE__, key);
// #define VALIDATE_MESSAGE(key, type) if (!unwrappedMessage.contains(## key) || unwrappedMessage[key].as ## type().isErr()) {msg->Release();continue;}