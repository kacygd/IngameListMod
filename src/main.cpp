#include <Geode/Geode.hpp>
#include <Geode/Modify.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) { // Truyền đủ 2 tham số
        if (!LevelInfoLayer::init(level, challenge)) return false;

        std::string url = "https://cps.ps.fhgdps.com/database/demonlist.php?levelID=" + std::to_string(level->m_levelID);

        // Sử dụng WebRequest thay vì AsyncWebRequest
        WebRequest()
            .get(url)
            .text() // Đọc dữ liệu dạng text
            .then([this](std::string body) {
                // Kiểm tra nếu dữ liệu hợp lệ
                if (body.find("Error") == std::string::npos) {
                    int rank = std::stoi(body); // Chuyển từ chuỗi sang số nguyên
                    
                    // Hiển thị rank trong level
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
