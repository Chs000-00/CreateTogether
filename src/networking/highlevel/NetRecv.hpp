#pragma once

#include "SharedHighLevelHeaders.hpp"

Result<uint8_t> recvCreateObjects(const CTSerialize::CreateObjects* msg);
