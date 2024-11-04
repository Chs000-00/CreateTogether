#include <Geode/Geode.hpp>

using namespace geode::prelude;


// specify parameters for the setup function in the Popup<...> template
class CreateLobbyPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override {
        // convenience function provided by Popup
        // for adding/setting a title to the popup
        this->setTitle("Create Lobby:");

        auto label = CCLabelBMFont::create(value.c_str(), "bigFont.fnt");
        m_mainLayer->addChildAtPosition(label, Anchor::Center);

        return true;
    }

public:
    static CreateLobbyPopup* create(std::string const& text) {
        auto ret = new CreateLobbyPopup();
        if (ret->initAnchored(240.f, 160.f, text)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};
