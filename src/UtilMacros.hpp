#pragma once

#include "config.hpp"

#define GET_OBJECT_FROM_UID (GameObject*)level->m_fields->m_pUniqueIDOfGameObject->objectForKey(unwrappedMessage["ObjectUID"].asString().ok().value())
#define GET_CCPOINT static_cast<float>(unwrappedMessage["x"].asInt().ok().value()), static_cast<float>(unwrappedMessage["y"].asInt().ok().value())

#define VALIDATE_MESSAGE(key, type) if (!unwrappedMessage.contains(## key) || unwrappedMessage[key].as ## type().isErr()) {msg->Release();continue;}
