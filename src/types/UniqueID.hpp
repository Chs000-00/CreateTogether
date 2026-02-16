#pragma once

#include <Geode/Geode.hpp>
#include "gdserialization_generated.h"

using namespace geode::prelude;


struct vUIDContainer {
    uint64_t associatedID;
    uint32_t objectID;

    vUIDContainer(uint64_t aID, uint32_t oID) {
        this->associatedID = aID;
        this->objectID = oID;
    }

    // TODO: Finish
    inline std::string getHash() {
        return ""
    }

    flatbuffers::Offset<CTSerialize::veryUniqueID> createOffset(flatbuffers::FlatBufferBuilder builder) {
        return CTSerialize::CreateveryUniqueID(builder, this->associatedID, this->objectID);
    }
};
