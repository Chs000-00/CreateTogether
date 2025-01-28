#pragma once

#include "config.hpp"


// TODO: STOP USING MACROS!

#define GET_OBJECT_FROM_UID static_cast<GameObject*>(level->m_fields->m_pUniqueIDOfGameObject->objectForKey(data["ObjectUID"].asString().ok().value()))
#define GET_CCPOINT static_cast<float>(unwrappedMessage["x"].asInt().ok().value()), static_cast<float>(unwrappedMessage["y"].asInt().ok().value())

// log::error("Message Invalidated at line {} and key {}", __LINE__, key);
// #define VALIDATE_MESSAGE(key, type) if (!unwrappedMessage.contains(## key) || unwrappedMessage[key].as ## type().isErr()) {msg->Release();continue;}