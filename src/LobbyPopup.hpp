#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// specify parameters for the setup function in the Popup<...> template
class LobbyPopup : public geode::Popup<std::string const&> {
private:
    bool setup(std::string const& value) override;

public:
    static LobbyPopup* create(std::string const& text);
};
