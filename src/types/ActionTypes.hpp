#pragma once

enum EEditorActionType {
    eActionPlacedObject,       // Object was placed in level
    eActionTransformObject,    // TransformObject was called, so the object was moved, scaled, etc
    eActionMovedObject,
    eActionScaledObject,       // Object was scaled
    eActionDeletedObject,      // Object was deleted
    eActionPastedObjects,      // Objects were pasted
    // This should always equal to 6 or the hardcoded values would not like that!
    eActionRequestLevel = 6,       // Request level for synchronization
    eActionReturnLevelString,  // Return level string which was requested
    eActionReturnProperUID,    // Return UID to be used for a newly created gameObject. Not implemented yet!
    eActionUpdatedColor,       // Color was updated
    eActionModifiedObject,     // Object data was changed
    eActionSongChanged,        // Level's song was changed
    eOptionSpeedChanged,       // Starting player speed was changed
    eOptionGameTypeChanged,    // Classic or platformer was changed
    eOptionGameModeChanged,    // Starting GameMode was changed
    eActionUpdatedFont,        // Level's font was updated/changed
    eActionDeleteAllStartpos   // Delete all startpos was pressed
};

