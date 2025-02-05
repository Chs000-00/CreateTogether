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
        case 1:
            settings->m_startDual = !settings->m_startDual;
            break;
        
        default:
            return Err("no corresponding option found");
            break;
    }
    return Ok();
}