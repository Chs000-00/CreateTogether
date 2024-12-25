#pragma once

enum EEditorActionType {
    eActionPlacedObject,       // Object was placed in level
    eActionSelectedObject,     // Object was selected
    eActionTransformObject,    // TransformObject was called, so the object was moved, scaled, etc
    eActionMovedObject,        // Object was moved
    eActionScaledObject,       // Object was scaled
    eActionDeletedObject,      // Object was deleted
    eActionPastedObjects,      // Objects were pasted
    // This should always equal to 7 or the hardcoded values would not like that!
    eActionRequestLevel = 7,   // Request level for synchronization
    eActionReturnLevelString,  // Return level string which was requested. Only host should be sending this!
    eActionUpdatedColor,       // Color was updated
    eActionModifiedObject,     // Object data was changed
    eActionSongChanged,        // Level's song was changed
    eOptionSpeedChanged,       // Starting player speed was changed
    eOptionGameTypeChanged,    // Classic or platformer was changed
    eOptionGameModeChanged,    // Starting GameMode was changed
    eActionUpdatedFont,        // Level's font was updated/changed
    eActionDeleteAllStartpos,  // Delete all startpos was pressed
    eActionLockLayer           // Host locks a layer
};

