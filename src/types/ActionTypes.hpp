#pragma once

enum EEditorActionType {
    eActionPlacedObject,       // Object was placed in level (Implemented)
    eActionSelectedObject,     // Object was selected
    eActionTransformObject,    // TransformObject was called, as in the green buttons in the edit tab (Implemented)
    eActionMovedObject,        // Object was moved (Implemented)
    eActionScaledObject,       // Object was scaled
    eActionDeletedObject,      // Object was deleted (Implemented)
    eActionPastedObjects,      // Objects were pasted
    // This should always equal to 7 or the hardcoded values would not like that!
    eActionRequestLevel = 7,   // Request level for synchronization
    eActionReturnLevelString,  // Return level string which was requested. Only host should be sending this!
    eActionUpdatedColor,       // Color was updated
    eActionModifiedObject,     // Object data was changed, used for triggers
    eActionSongChanged,        // Level's song was changed (Almost Implemented)
    eOptionSpeedChanged,       // Starting player speed was changed
    eOptionGameTypeChanged,    // Classic or platformer was changed
    // Line change might be moved to another action type.
    eActionArtSelected,        // Background art, ground art, or line was changed. (Needs testing)
    eOptionGameModeChanged,    // Starting GameMode was changed
    eActionUpdatedFont,        // Level's font was updated/changed (Implemented)
    eActionDeleteAllStartpos,  // Delete all startpos was pressed
    eActionLockLayer           // Host locks a layer
};

