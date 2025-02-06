#include <Geode/Geode.hpp>
#include <Geode/Modify.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;
using geode::utils::web::WebRequest; // Thêm dòng này để sử dụng WebRequest

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) { // Truyền đủ 2 tham số
        if (!LevelInfoLayer::init(level, challenge)) return false;

        std::string url = "https://cps.ps.fhgdps.com/database/demonlist.php?levelID=" + std::to_string(level->m_levelID);

        // Sử dụng WebRequest với đầy đủ namespace
        WebRequest()
            .get(url)
            .text() // Đọc dữ liệu dạng text
            .then([this](std::string body) {
                if (body.find("Error") == std::string::npos) {
                    int rank = std::stoi(body);
                    
                    auto rankLabel = CCLabelBMFont::create(
                        ("Rank: " + std::to_string(rank)).c_str(),
                        "bigFont.fnt"
                    );
                    rankLabel->setPosition(100, 150); // Điều chỉnh vị trí phù hợp
                    this->addChild(rankLabel);
                }
            });

        return true;
    }
};
