#include <ctserialize_generated.h>
#include <ctcursor_generated.h>
#include "../../types/Cursor.hpp"


class CursorManager {
    public:

        static CursorManager* get();

        void sendCursorUpdateToAll();
        
        std::vector<CreateTogetherCursor*> m_playerCursors;

        // TODO: Update func
};