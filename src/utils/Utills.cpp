#include "Utills.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;



// Checks if a enum is within range.
// Comparison uses <=
bool isValidEnumRange(int n, int min, int max) {
    return (min <= n && n <= max);
}

Result<int> toggleFromLevelSettings(LevelSettingsObject* settings, int option) {
    switch (option) {
        case 0:
            settings->m_startMini = !settings->m_startMini;
            break;

        case 1:
            settings->m_startDual = !settings->m_startDual;
            break; 

        case 2:
            settings->m_twoPlayerMode = !settings->m_twoPlayerMode;
            break;

        case 4:
            settings->m_mirrorMode = !settings->m_mirrorMode;
            break;

        case 5:  
            settings->m_isFlipped = !settings->m_isFlipped;
            break;

        case 6:
            settings->m_rotateGameplay = !settings->m_rotateGameplay;
            break;

        case 7:
            settings->m_reverseGameplay = !settings->m_reverseGameplay;
            break;

        case 8:
            settings->m_allowMultiRotation = !settings->m_allowMultiRotation;
            break;

        case 9:
            settings->m_enablePlayerSqueeze = !settings->m_enablePlayerSqueeze;
            break;
        
        case 10:
            settings->m_fixGravityBug = !settings->m_fixGravityBug;
            break;

        case 11:
            settings->m_fixNegativeScale = !settings->m_fixNegativeScale;
            break;

        case 12:
            settings->m_fixRobotJump = !settings->m_fixRobotJump;
            break;

        case 13:
            return Err("toggleFromLevelSettings: Case 13 is invalid and shouldn't have been sleected");
            break; // Just in case

        case 14:
            settings->m_dynamicLevelHeight = !settings->m_dynamicLevelHeight;
            break;

        case 15:
            settings->m_sortGroups = !settings->m_sortGroups;
            break;

        case 16:
            settings->m_fixRadiusCollision = !settings->m_fixRadiusCollision;
            break;

        case 17:
            settings->m_enable22Changes = !settings->m_enable22Changes;
            break;

        case 18:
            settings->m_allowStaticRotate = !settings->m_allowStaticRotate;
            break;

        case 19:
            settings->m_reverseSync = !settings->m_reverseSync;
            break;

        case 20:
            settings->m_noTimePenalty = !settings->m_noTimePenalty;
            break;

        case 22:
            settings->m_propertykA45 = !settings->m_propertykA45;
            break;

        
        default:
            return Err("toggleFromLevelSettings: No corresponding option found");
            break;
    }
    return Ok(0);
}