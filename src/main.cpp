#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Loader.hpp>

using namespace geode::prelude;

std::unordered_map<int, int> cachedPositions;
bool initialized = false;

EventListener<web::WebTask> webRequest;

void createButton(CCLayer* self, CCLabelBMFont* label, int levelID) {
    CCPoint position = { label->getPositionX() + 8.f, label->getPositionY() };
    auto button = CCMenuItemSpriteExtra::create(label, self, menu_selector([](CCObject* ret) {
        int levelID = static_cast<CCInteger*>(static_cast<CCNode*>(ret)->getUserObject())->getValue();
        std::string url = "https://cps.ps.fhgdps.com/database/data/levels/" + std::to_string(levelID);
        web::openLinkInBrowser(url.c_str());
    }));
    auto menu = CCMenu::create();
    menu->setPosition(position);
    menu->addChild(button);
    self->addChild(menu);
}

void getRequest(CCLayer* self, GJGameLevel* level, CCLabelBMFont* label) {
    self->retain();  // Giữ lại self trong khi yêu cầu web chưa hoàn thành
    int levelID = level->m_levelID;
    std::string url = "https://cps.ps.fhgdps.com/database/demonlist.php?levelID=" + std::to_string(levelID);

    webRequest.bind([self, label, levelID](web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            std::string result = res->string().unwrap();
            int position = std::stoi(result);

            if (position > 0) {
                label->setString(std::to_string(position).c_str());
                createButton(self, label, levelID);
                cachedPositions.insert({ levelID, position });
            } else {
                label->setString("N/A");
                cachedPositions.insert({ levelID, -1 });
            }
        } else if (e->isCancelled()) {
            label->setString("???");
        }
        self->autorelease();
    });

    auto req = web::WebRequest();
    webRequest.setFilter(req.get(url));
}

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* level, bool idk) {
        if (!LevelInfoLayer::init(level, idk)) return false;

        if (level->m_demon != 1) return true;  // Nếu không phải level demon, bỏ qua

        int offset = (level->m_coins == 0) ? 17 : 4;
        auto director = CCDirector::sharedDirector();
        auto size = director->getWinSize();
        auto it = cachedPositions.find(level->m_levelID);

        CCLabelBMFont* label = CCLabelBMFont::create(" ... ", "goldFont.fnt");
        label->setPosition({ size.width / 2 - 100, size.height / 2 + offset });
        label->setScale(0.5f);

        if (it != cachedPositions.end()) {
            if (cachedPositions[level->m_levelID] > -1) {
                int position = cachedPositions[level->m_levelID];
                label->setString(std::to_string(position).c_str());
                createButton(this, label, level->m_levelID);
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
