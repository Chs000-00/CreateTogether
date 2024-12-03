#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>

using namespace geode::prelude;

class $modify(MyGameObject, GameObject) {

    struct Fields {
        // Was the data sent from another user?
        bool m_wasDataSent = false;

        unsigned int m_veryUniqueID = 0;
    };

    void destroyObject();
    void setPosition(const CCPoint& p0);
    bool initWithTexture(cocos2d::CCTexture2D* p0);
};
