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
            .into_string()  // Thay thế text() bằng into_string()
            .then([this](std::string body) {
                if (body.find("Error") == std::string::npos) {
                    int rank = std::stoi(body);
                    
                    auto rankLabel = CCLabelBMFont::create(
                        ("Rank: " + std::to_string(rank)).c_str(),
                        "bigFont.fnt"
                    );
                    rankLabel->setPosition(100, 150);
                    this->addChild(rankLabel);
                }
            });

        return true;
    }
};
