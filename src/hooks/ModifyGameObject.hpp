#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>

using namespace geode::prelude;

class $modify(MyGameObject, GameObject) {

    struct Fields {
        // // Was the data sent from another user?
        // bool m_wasDataSent = false;
        std::string m_veryUniqueID;
    };
};
