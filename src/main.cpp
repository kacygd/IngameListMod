#include <Geode/Geode.hpp>
#include <Geode/Modify.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;
using geode::utils::web::WebRequest;

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        std::string url = "https://cps.ps.fhgdps.com/database/demonlist.php?levelID=" + std::to_string(level->m_levelID);

        WebRequest()
            .get(url)
            .json([this](std::optional<matjson::Value> result) { // Dùng json() thay vì text()
                if (result.has_value() && result->is_number()) { // Kiểm tra nếu kết quả là số
                    int rank = result->as<int>(); // Chuyển thành số nguyên
                    
                    auto rankLabel = CCLabelBMFont::create(
                        ("Rank: " + std::to_string(rank)).c_str(),
                        "bigFont.fnt"
                    );
                    rankLabel->setPosition(100, 150);
                    CCDirector::sharedDirector()->getRunningScene()->addChild(rankLabel);
                }
            });

        return true;
    }
};
