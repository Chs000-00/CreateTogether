#pragma once

// TODO: Fix eOption messages "not working" if optionsLayer is open.
enum EEditorActionType {
    eActionPlacedObject,       // Object was placed in level (Implemented)
    eActionSelectedObject,     // Object was selected
    eActionTransformObject,    // TransformObject was called, as in the green buttons in the edit tab (Implemented)
    eActionMovedObject,        // Object was moved (Implemented)
    eActionScaledObject,       // Object was scaled
    eActionDeletedObject,      // Object was deleted (Implemented)
    eActionPastedObjects,      // Objects were pasted
    // This should always equal to 7 or the hardcoded values would not like that!
    eActionRequestLevel = 7,   // Request level for synchronization (Needs testing)
    eActionReturnLevelString,  // Return level string which was requested. Only host should be sending this! (Needs testing)
    eActionUpdatedColor,       // Color was updated
    // eActionModifiedObject,     // Object data was changed, used for triggers
    eActionEnchancedObjectEdited, // Enchanced gameObject was changed
    eActionAltGameObjectEdited,// ):
    eActionEffectObjectEdited, // Effect gameObject was changed
    eActionSongChanged,        // Level's song was changed (Implemented)
    eOptionSpeedChanged,       // Starting player speed was changed (Implemented)
    eOptionGameTypeChanged,    // Classic or platformer was changed
    // Line change might be moved to another action type.
    eActionArtSelected,        // Background art, ground art, or line was changed. (Needs testing)
    eOptionGameModeChanged,    // Starting GameMode was changed (Implemented)
    eActionUpdatedFont,        // Level's font was updated/changed (Implemented)
    // eActionDeleteAllStartpos,  // Delete all startpos was pressed
    eActionLockLayer,          // Host locks a layer, clients cannot unlock a layer (?)
};

