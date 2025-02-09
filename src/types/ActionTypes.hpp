#pragma once

// TODO: Fix eOption messages "not working" if optionsLayer is open.
enum EEditorActionType {
    eActionPlacedObject,       // Object was placed in level (Implemented)
    eActionSelectedObject,     // Object was selected
    eActionTransformObject,    // TransformObject was called, as in the green buttons in the edit tab (Implemented)
    eActionMovedObject,        // Object was moved (Implemented)
    eActionScaledObject,       // Object was scaled
    eActionDeletedObject,      // Object was deleted (Implemented)
    eActionPastedObjects,      // Objects were copied + pasted (Implemented)
    // This should always equal to 7 or the hardcoded values would not like that!
    eActionRequestLevel = 7,   // Request level for synchronization (Objects sync)
    eActionReturnLevelString,  // Return level string which was requested. Only host should be sending this! (Implemented)
    eActionChangeGroupID,      // Group ID was modified (Implemented)
    eActionUpdatedColor,       // Color was updated
    eActionModifiedObject,     // Object data was changed, used for EffectGameObjects and EnhancedGameObjects (Implemented)
    eActionSongChanged,        // Level's song was changed (Implemented)
    eOptionSpeedChanged,       // Starting player speed was changed (Implemented)
    eOptionGameTypeChanged,    // Classic or platformer was changed
    // Line change might be moved to another action type.
    eActionArtSelected,        // Background art, ground art, or line was changed. (Implemented)
    eOptionLevelSetting,       // Level setting was changed
    eOptionGameModeChanged,    // Starting GameMode was changed (Implemented)
    eActionUpdatedFont,        // Level's font was updated/changed (Implemented)
    eActionDeleteAllOfType,    // Delete all objects of a certain ID
    eActionLockLayer,          // Host locks a layer, clients cannot unlock a layer (? not sure if I should implement this)
    // TODO: Move this higher upward
    eActionRotatedObject,
    eActionWeirdlyScaledObject // Like the object being scaled, but weird?
};

