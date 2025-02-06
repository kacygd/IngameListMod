#include <Geode/Geode.hpp>
#include <Geode/Modify.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* level) {
        if (!LevelInfoLayer::init(level)) return false;

        std::string url = "https://cps.ps.fhgdps.com/database/demonlist.php?levelID=" + std::to_string(level->m_levelID);

        geode::utils::web::AsyncWebRequest::get(url, [this](const geode::utils::web::Response& response) {
            if (response.isSuccessful()) {
                std::string body = response.getBody();
                
                // Kiểm tra nếu dữ liệu là số (rank)
                if (body.find("Error") == std::string::npos) {
                    int rank = std::stoi(body); // Chuyển từ string sang số nguyên
                    
                    // Hiển thị rank trong level
                    auto rankLabel = CCLabelBMFont::create(
                        ("Rank: " + std::to_string(rank)).c_str(),
                        "bigFont.fnt"
                    );
                    rankLabel->setPosition(100, 150); // Điều chỉnh vị trí phù hợp
                    this->addChild(rankLabel);
                }
            }
        });

        return true;
    }
};
