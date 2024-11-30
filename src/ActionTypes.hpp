#pragma once

enum EEditorActionType {
    eActionPlacedObject,       // Object was placed in level
    eActionMovedObject,        // Object was moved
    eActionRotatedObject,      // Object was rotated
    eActionDeletedObject,      // Object was deleted
    eActionPastedObjects,      // Objects were pasted
    eActionRequestLevel,       // Request level for synchronization
    eActionReturnLevelString,  // Return level string which was requested
    eActionUpdatedColor,       // Color was updated
    eActionModifiedObject,     // Object data was changed
    eActionSongChanged,        // Level's song was changed
    eOptionSpeedChanged,       // Starting player speed was changed
    eOptionGameTypeChanged,    // Classic or platformer was changed
    eOptionGameModeChanged,    // Starting GameMode was changed
    eActionUpdatedFont,        // Level's font was updated/changed
    eActionDeleteAllStartpos   // Delete all startpos was pressed
};

