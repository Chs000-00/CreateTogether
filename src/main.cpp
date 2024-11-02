#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>


using namespace geode::prelude;

class $modify(MyEditorPauseLayer, EditorPauseLayer) {

	bool init(LevelEditorLayer* p0) {

		if (!EditorPauseLayer::init(p0)) {
			return false;
		}


		auto hostButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("GJ_hammerIcon_001.png"),
			this,
			menu_selector(MyEditorPauseLayer::onHost)
		);

		auto menu = this->getChildByID("guidelines-menu");
		menu->addChild(hostButton);
		hostButton->setID("host-button"_spr);
		menu->updateLayout();
		return true;
	}

	void onHost(CCObject*) {
		FLAlertLayer::create("CreateTogether", "Might be hosting!", "OK")->show();
	}
};