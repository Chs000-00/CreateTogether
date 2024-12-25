#include "../include.hpp"

class $modify(MyGameObject, GameObject) {

    struct Fields {
        // // Was the data sent from another user?
        // bool m_wasDataSent = false;
        UUIDv4::UUID m_veryUniqueID;
    };
};
