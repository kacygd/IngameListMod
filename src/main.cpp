#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Loader.hpp>

using namespace geode::prelude;

std::unordered_map<int, int> cachedPositions;
bool requestFinished = false;
bool initialized = false;

EventListener<web::WebTask> webRequest;

void onButtonPressed(CCObject* sender) {
    // Hàm callback khi bấm vào nút
    CCLog("Button pressed!");
}

void createButton(CCLayer* self, CCLabelBMFont* label, int levelID, int rank) {
    CCPoint position = { label->getPositionX() + 8.f, label->getPositionY() };

    std::string texture;
    if (rank == 1) texture = "rankIcon_1_001.png";
    else if (rank <= 10) texture = "rankIcon_top10_001.png";
    else if (rank <= 20) texture = "rankIcon_top50_001.png";
    else if (rank <= 35) texture = "rankIcon_top100_001.png";
    else if (rank <= 50) texture = "rankIcon_top200_001.png";
    else if (rank <= 75) texture = "rankIcon_top500_001.png";
    else if (rank <= 100) texture = "rankIcon_top1000_001.png";
    else texture = "rankIcon_all_001.png";

    auto button = CCMenuItemSpriteExtra::create(label, self, menu_selector(onButtonPressed));
    auto menu = CCMenu::create();
    auto trophy = CCSprite::createWithSpriteFrameName(texture.c_str());
    trophy->setScale(0.5f);
    trophy->setPosition({ -10.f, 5.f });
    menu->addChild(button);
    menu->setPosition(position);
    button->addChild(trophy);
    self->addChild(menu);
}

void getRequest(CCLayer* self, GJGameLevel* level, CCLabelBMFont* label) {
    self->retain();
    int levelID = level->m_levelID;
    std::string url = "https://cps.ps.fhgdps.com/database/demonlist.php?levelID=" + std::to_string(levelID);

    webRequest.bind([self, label, levelID](web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            std::string result = res->string().unwrap();
            try {
                int position = std::stoi(result);
                if (position > 0) {
                    label->setString(std::to_string(position).c_str());
                    cachedPositions[levelID] = position;
                    createButton(self, label, levelID, position);
                } else {
                    label->setString("N/A");
                    cachedPositions[levelID] = -1;
                }
            } catch (const std::exception&) {
                label->setString("???");
            }
        } else {
            label->setString("???");
        }
        self->release();
    });

    auto req = web::WebRequest();
    webRequest.setFilter(req.get(url));
}

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* level, bool idk) {
        if (!LevelInfoLayer::init(level, idk)) return false;
        if (level->m_demon != 1) return true;

        int offset = (level->m_coins == 0) ? 17 : 4;
        auto director = CCDirector::sharedDirector();
        auto size = director->getWinSize();
        auto it = cachedPositions.find(level->m_levelID);

        CCLabelBMFont* label = CCLabelBMFont::create(" ... ", "goldFont.fnt");
        label->setPosition({ size.width / 2 - 100, size.height / 2 + offset });
        label->setScale(0.5f);

        if (it != cachedPositions.end()) {
            if (it->second > -1) {
                label->setString(std::to_string(it->second).c_str());
                createButton(this, label, level->m_levelID, it->second);
            } else {
                label->setString("N/A");
            }
        } else {
            getRequest(this, level, label);
        }

        addChild(label);
        return true;
    }
};

class $modify(MenuLayer) {
    bool init() {
        auto result = MenuLayer::init();
        if (initialized) return result;
        Loader* loader = Loader::get();
        if (loader->isModLoaded("gdutilsdevs.gdutils")) {
            loader->getLoadedMod("gdutilsdevs.gdutils")->setSettingValue<bool>("demonListPlacement", false);
        }
        initialized = true;
        return result;
    }
};
