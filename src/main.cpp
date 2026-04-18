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

std::string getSubfolderName(IconType type) {
	switch(type) {
		case IconType::Cube:
			return "icon";
		case IconType::Ship:
			return "ship";
		case IconType::Ball:
			return "ball";
		case IconType::Ufo:
			return "ufo";
		case IconType::Wave:
			return "wave";
		case IconType::Robot:
			return "robot";
		case IconType::Spider:
			return "spider";
		case IconType::Swing:
			return "swing";
		case IconType::Jetpack:
			return "jetpack";
		default:
			return "unknown";
	}
}

class $modify(StealerGarageLayer, GJGarageLayer) {
	static void onModify(auto& self) {
		if (!self.setHookPriorityPost("GJGarageLayer::init", Priority::Last)) {
			log::warn("Failed to set hook priority.");
		}
	}

	bool init() {
		if (!GJGarageLayer::init()) return false;

		auto winSize = CCDirector::sharedDirector()->getWinSize();
		auto btnSpr = CircleButtonSprite::createWithSprite("stealIcon.png"_spr, 0.85f, CircleBaseColor::Red, CircleBaseSize::MediumAlt);
		auto btn = CCMenuItemSpriteExtra::create(btnSpr, this, menu_selector(StealerGarageLayer::stealIcon));
		auto menu = CCMenu::create();

		menu->setContentSize(btn->getContentSize());
		menu->setAnchorPoint({0.5f, 0.5f});
		menu->addChild(btn);

		btn->setPosition({menu->getContentSize().width / 2.f, menu->getContentSize().height / 2.f});

		menu->setPosition({winSize.width - 50.f, winSize.height / 2.f - 60.f});
		menu->setScale(0.7f);

		this->addChild(menu);

		return true;
	}

	void stealIcon(CCObject* sender) {
		IconType stealType = GameManager::sharedState()->m_playerIconType;
		IconInfo* icon = more_icons::activeIcon(stealType);

		if (icon->isVanilla() || icon->isZipped()) {
			FLAlertLayer::create(
				"Illegal Icon",
				"The icon you're trying to steal is either a Vanilla Icon or part of a Zipped texture pack, so it is unaccessible! Please select another icon within a proper, unzipped texture pack.",
				"OK"
			)->show();
			return;
		}

		if (!Mod::get()->setSavedValue("shown-first-warning", true)) {
			FLAlertLayer::create(
				"WARNING!",
				"In case you haven't, PLEASE visit the mod's settings and set your Icon destination folder! Otherwise the mod can crash or save your icons god knows where.",
				"OK"
			)->show();
			return;
		}

		std::string iconName = icon->getShortName();
		std::string typeName = getSubfolderName(stealType);
		std::string plistFileName = icon->getShortName() + getQualitySuffix(icon->getQuality()) + ".plist";
		std::string sheetFileName = icon->getShortName() + getQualitySuffix(icon->getQuality()) + ".png";

		std::string iconFolder = utils::string::trimRight(icon->getSheetString(), plistFileName); 
		std::string targetPathString = utils::string::pathToString(Mod::get()->getSettingValue<std::filesystem::path>("destination-folder")) + "\\" + typeName + "\\";

		std::filesystem::path plistPath = icon->getSheet();
		std::filesystem::path sheetPath = iconFolder + sheetFileName;

		std::filesystem::path trueTargetFolder = targetPathString;
		std::filesystem::path newPlistPath = targetPathString + plistFileName;
		std::filesystem::path newSheetPath = targetPathString + sheetFileName;

		if (!std::filesystem::is_directory(Mod::get()->getSettingValue<std::filesystem::path>("destination-folder"))) {
			FLAlertLayer::create(
				"Folder Not Found!",
				"The folder you're trying to save icons to doesn't exist, are you sure you set it up correctly in mod settings? (Default folder is only created when you open the mod's config directory, btw...)",
				"OK"
			)->show();
			return;
		}

		std::filesystem::create_directory(trueTargetFolder);

		std::filesystem::copy(plistPath, newPlistPath, std::filesystem::copy_options::update_existing);
		std::filesystem::copy(sheetPath, newSheetPath, std::filesystem::copy_options::update_existing);

		Notification::create("Icon stolen!", CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"), 0.3f)->show();
	}
};