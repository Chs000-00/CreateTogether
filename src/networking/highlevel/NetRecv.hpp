#pragma once

#include "SharedHighLevelHeaders.hpp"

Result<int> recvCreateObjects(const CTSerialize::MessageHeader* msg);
