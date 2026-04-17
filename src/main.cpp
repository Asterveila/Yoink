#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <hiimjustin000.more_icons/include/MoreIcons.hpp>

using namespace geode::prelude;

std::string getQualitySuffix(int miQuality) {

	switch(miQuality) {
		case 1:
			return "";
		case 2:
			return "-hd";
		case 3:
			return "-uhd";
		default:
			return "what the fuck are you doing";
	}
}

class $modify(StealerGarageLayer, GJGarageLayer) {
	bool init() {
		if (!GJGarageLayer::init()) return false;

		auto winSize = CCDirector::sharedDirector()->getWinSize();
		auto btnSpr = CircleButtonSprite::create(CCSprite::create("stealIcon.png"_spr), CircleBaseColor::Red, CircleBaseSize::MediumAlt);
		auto btn = CCMenuItemSpriteExtra::create(btnSpr, this, menu_selector(StealerGarageLayer::stealIcon));
		auto menu = CCMenu::create();

		menu->setContentSize(btn->getContentSize());
		menu->setAnchorPoint({0.5f, 0.5f});
		menu->addChild(btn);

		btn->setPosition({menu->getContentSize().width / 2.f, menu->getContentSize().height / 2.f});

		menu->setPosition({10.f, 40.f});
		menu->setScale(0.8f);

		this->addChild(menu);

		return true;
	}

	void stealIcon(CCObject* sender) {
		IconInfo* icon = more_icons::activeIcon(GameManager::sharedState()->m_playerIconType);

		if (icon->isVanilla() || icon->isZipped()) {
			FLAlertLayer::create(
				"Illegal Icon",
				"The icon you're trying to steal is either a Vanilla Icon or part of a Zipped texture pack, so it is unaccessible! Please select another icon within a proper, unzipped texture pack.",
				"OK"
			)->show();
			return;
		}

		std::string iconName = icon->getShortName();
		std::string plistFileName = icon->getShortName() + getQualitySuffix(icon->getQuality()) + ".plist";
		std::string sheetFileName = icon->getShortName() + getQualitySuffix(icon->getQuality()) + ".png";

		std::string iconFolder = utils::string::trimRight(icon->getSheetString(), plistFileName); 
		std::string targetPathString = utils::string::pathToString(Mod::get()->getSettingValue<std::filesystem::path>("destination-folder")) + "\\";

		std::filesystem::path plistPath = icon->getSheet();
		std::filesystem::path sheetPath = iconFolder + sheetFileName;

		std::filesystem::path newPlistPath = targetPathString + plistFileName;
		std::filesystem::path newSheetPath = targetPathString + sheetFileName;

		std::filesystem::copy(plistPath, newPlistPath, std::filesystem::copy_options::update_existing);
		std::filesystem::copy(sheetPath, newSheetPath, std::filesystem::copy_options::update_existing);

		auto sigh = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
		Notification::create("Icon stolen!", sigh, 0.3f)->show();
	}
};