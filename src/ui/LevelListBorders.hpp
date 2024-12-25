#pragma once

#include "../include.hpp"


using namespace geode::prelude;

// Creates a border similar to the ones when browsing levels
class GDLevelListBorders : public ListBorders {
protected:
    bool init() override;
public:
    static GDLevelListBorders* create();
    void setPadding(float horizontalPadding);
    void setContentSize(CCSize const& size) override;
};
