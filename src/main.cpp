#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Loader.hpp>

using namespace geode::prelude;

std::unordered_map<int, int> cachedPositions;
bool initialized = false;

EventListener<web::WebTask> webRequest;

std::string getRankTexture(int rank) {
    if (rank == 1) return "rankIcon_1_001.png";
    if (rank <= 10) return "rankIcon_top10_001.png";
    if (rank <= 20) return "rankIcon_top50_001.png";
    if (rank <= 35) return "rankIcon_top100_001.png";
    if (rank <= 50) return "rankIcon_top200_001.png";
    if (rank <= 75) return "rankIcon_top500_001.png";
    if (rank <= 100) return "rankIcon_top1000_001.png";
    return "rankIcon_all_001.png";
}

void createButton(CCLayer* self, CCLabelBMFont* label, int levelID, int rank) {
    CCPoint position = { label->getPositionX() + 8.f, label->getPositionY() };
    auto button = CCMenuItemSpriteExtra::create(label, self, nullptr);
    auto menu = CCMenu::create();
    auto trophy = CCSprite::createWithSpriteFrameName(getRankTexture(rank).c_str());
    trophy->setScale(0.5f);
    trophy->setPosition({ -10.f, 5.f });
    menu->addChild(button);
    menu->setPosition(position);
    button->addChild(trophy);
    self->addChild(menu);
}


void getRequest(CCLayer* self, GJGameLevel* level, CCLabelBMFont* label) {
    int levelID = level->m_levelID;
    std::string url = "https://cps.ps.fhgdps.com/database/demonlist.php?levelID=" + std::to_string(levelID);

    webRequest.bind([self, label, levelID](web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            std::string result = res->string().unwrap();
            try {
                int position = std::stoi(result);
                cachedPositions[levelID] = (position > 0) ? position : -1;
                label->setString((position > 0) ? std::to_string(position).c_str() : "N/A");
                if (position > 0) createButton(self, label, levelID);
            } catch (...) {
                label->setString("N/A");
                cachedPositions[levelID] = -1;
            }
        } else {
            label->setString("???");
        }
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
            label->setString((it->second > -1) ? std::to_string(it->second).c_str() : "N/A");
            if (it->second > -1) createButton(this, label, level->m_levelID);
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
