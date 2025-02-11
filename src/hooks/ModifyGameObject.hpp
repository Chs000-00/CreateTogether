#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>

using namespace geode::prelude;

class $modify(MyGameObject, GameObject) {
    struct Fields {
        // // Was the data sent from another user?
        // bool m_wasDataSent = false;

        // m_veryUniqueID is a unique id assigned to every gameObject. You can access an objects vUID and an object from a vUID via dictionary inside the EditorLayer
        // vUID can be **any** unique identifier, currently it is the user who created it steam ID + "!" + a counter which increments each time said person places a block down 
        std::string m_veryUniqueID;
    };
};
