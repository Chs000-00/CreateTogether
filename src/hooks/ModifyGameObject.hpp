#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>

using namespace geode::prelude;

class $modify(MyGameObject, GameObject) {

    struct Fields {
        // Was the data sent from another user?
        bool m_wasDataSent = false;

        UUIDv4::UUID m_veryUniqueID;
    };

    void destroyObject();
    void setPosition(const CCPoint& p0);
};
