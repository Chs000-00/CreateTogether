#pragma once

#include "SharedHighLevelHeaders.hpp"

Result<uint8_t> recvCreateObjects(const CTSerialize::CreateObjects* msg);

Result<uint8_t> recvDeleteObjects(const CTSerialize::DeleteObjects* msg);

Result<uint8_t> recvMoveObjects(const CTSerialize::MoveObjects* msg);