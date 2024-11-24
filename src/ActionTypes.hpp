#pragma once

enum action_types {
    eActionRequestLevel,  // Request level for synchronization
    eActionPlacedObject,  // Object was placed in level
    eActionUpdatedFont,   // Font was Updated/Changed
    eActionUpdateColor,   // Color was updated
    eActionDeletedObject, // Object was deleted
};