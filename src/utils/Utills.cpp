#include "Utills.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;



// Checks if a enum is within range.
// Comparison uses <=
bool isValidEnumRange(int n, int min, int max) {
    return (min <= n && n <= max);
}


Result<void> toggleFromLevelSettings(LevelSettingsObject* settings, int option) {
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
        
        default:
            return Err("no corresponding option found");
            break;
    }
    return Ok();
}