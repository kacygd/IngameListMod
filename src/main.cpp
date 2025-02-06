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

class DemonClass {
public:
    void showInfoBox(CCObject* sender);
    void showInternetFail(CCObject* sender);
    void openLink(CCObject* ret);
};

void createButton(CCLayer* self, CCLabelBMFont* label, int levelID) {
    CCPoint position = { label->getPositionX() + 8.f, label->getPositionY() };
    auto button = CCMenuItemSpriteExtra::create(label, self, menu_selector(DemonClass::openLink));
    auto menu = CCMenu::create();
    auto trophy = CCSprite::createWithSpriteFrameName("rankIcon_top50_001.png");
    trophy->setScale(0.5f);
    trophy->setPosition({ -10.f, 5.f });
    menu->addChild(button);
    menu->setPosition(position);
    button->addChild(trophy);
    self->addChild(menu);
}

void DemonClass::openLink(CCObject* ret) {
    int levelID = static_cast<CCInteger*>(static_cast<CCNode*>(ret)->getUserObject())->getValue();
    std::string url = "https://cps.ps.fhgdps.com/level/" + std::to_string(levelID);
    web::openLinkInBrowser(url.c_str());
}

void infoButton(CCLayer* layer, CCLabelBMFont* label, bool internetFail = false) {
    CCPoint position = { label->getPositionX() - 122, label->getPositionY() - 81 };
    auto buttonBg = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    auto button = CCMenuItemSpriteExtra::create(buttonBg, layer, internetFail ? menu_selector(DemonClass::showInternetFail) : menu_selector(DemonClass::showInfoBox));
    auto menu = CCMenu::create();
    menu->setScale(0.5f);
    menu->addChild(button);
    menu->setPosition(position);
    layer->addChild(menu);
}

void DemonClass::showInfoBox(CCObject* sender) {
    FLAlertLayer::create("Thông tin vị trí N/A", "Mức độ này chưa bao giờ được xếp hạng hoặc chưa được đặt trên danh sách.", "OK")->show();
}

void DemonClass::showInternetFail(CCObject* sender) {
    FLAlertLayer::create("Lỗi kết nối", "Không thể tìm thấy thứ hạng của mức độ này. Vui lòng kiểm tra kết nối Internet của bạn.", "OK")->show();
}

void getRequest(CCLayer* self, GJGameLevel* level, CCLabelBMFont* label) {
    self->retain();
    int levelID = level->m_levelID;
    std::string url = "https://cps.ps.fhgdps.com/database/demonlist.php?levelID=" + std::to_string(levelID);

    webRequest.bind([self, label, levelID](web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            std::string result = res->string().unwrap();
            int position = std::stoi(result);
            if (position > 0) {
                label->setString(std::to_string(position).c_str());
                auto pos = CCInteger::create(levelID);
                createButton(self, label, levelID);
                cachedPositions.insert({ levelID, position });
            } else {
                label->setString("N/A");
                infoButton(self, label);
                cachedPositions.insert({ levelID, -1 });
            }
        } else if (e->isCancelled()) {
            label->setString("???");
            infoButton(self, label, true);
        }
        self->autorelease();
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
            if (cachedPositions[level->m_levelID] > -1) {
                int position = cachedPositions[level->m_levelID];
                label->setString(std::to_string(position).c_str());
                createButton(this, label, level->m_levelID);
            } else {
                label->setString("N/A");
                infoButton(this, label);
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
