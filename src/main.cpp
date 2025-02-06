#include <Geode/Geode.hpp>
#include <Geode/Modify.hpp>
#include <Geode/utils/web.hpp>
using namespace geode::prelude;
using geode::utils::web::WebRequest;

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        std::string url = "https://cps.ps.fhgdps.com/database/demonlist.php?levelID=" + std::to_string(level->m_levelID);

        // Gửi yêu cầu web để lấy rank của level
        WebRequest()
            .get(url)
            .await([this](geode::utils::web::WebResponse response) {
                if (response.status != 200) {
                    // Xử lý lỗi nếu yêu cầu thất bại
                    log::error("Failed to fetch rank: HTTP {}", response.status);
                    return;
                }

                // Kiểm tra nếu dữ liệu hợp lệ (chỉ chứa số)
                auto body = response.body;
                if (!body.empty() && std::all_of(body.begin(), body.end(), ::isdigit)) {
                    int rank = std::stoi(body);

                    // Hiển thị rank trên UI
                    auto rankLabel = CCLabelBMFont::create(
                        ("Rank: " + std::to_string(rank)).c_str(),
                        "bigFont.fnt"
                    );
                    rankLabel->setPosition(100, 150); // Điều chỉnh vị trí nếu cần
                    this->addChild(rankLabel);
                } else {
                    log::error("Invalid rank data: {}", body);
                }
            });

        return true;
    }
};
