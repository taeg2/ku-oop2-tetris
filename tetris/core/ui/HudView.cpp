#include "ui/HudView.h"
#include "common/TextUtil.h"
#include "common/GameConfig.h"

HudView::HudView(Point origin, const StageSession& session) : View(origin), m_session(session) {}

void HudView::draw(Console& console) const {
    const int x = m_origin.x;
    int y = m_origin.y;
    const StageConfig& config = m_session.config();
    console.draw({x, y++}, config.title, Color::White);
    console.draw({x, y++}, config.period, Color::Yellow);
    ++y;

    const PhotoProgress& progress = m_session.progress();
    console.draw({x, y}, "사진 " + std::to_string(progress.pieces()) + "/" + std::to_string(config::kPhotoPieces),
                 Color::White);
    for (int i = 0; i < config::kPhotoPieces; ++i) {
        const Color c = i < progress.pieces() ? Color::Yellow : Color::DarkGray;
        console.fill({x + 9 + i * 2, y, 1, 1}, c);
    }
    ++y;
    if (!progress.isComplete()) {
        console.draw({x, y}, "조각까지 " + std::to_string(progress.linesUntilNextPiece()) + "줄", Color::Gray);
    }
    ++y;

    const SpeedController& speed = m_session.speed();
    console.draw({x, y}, "속도 Lv." + std::to_string(speed.level() + 1), Color::Gray);
    if (speed.isSlowed()) {
        console.draw({x + 10, y}, "느림" + std::to_string((speed.slowLeftMs() + 999) / 1000), Color::Cyan);
    }
    y += 2;
    drawPills(console, y);
    drawBoss(console, y + 4);
}

void HudView::drawPills(Console& console, int y) const {
    const int x = m_origin.x;
    console.draw({x, y}, "약 가방", Color::White);
    const Inventory& inventory = m_session.inventory();
    const int blue = inventory.count(PillType::Blue);
    const int red = inventory.count(PillType::Red);
    console.draw({x, y + 1}, "[1] 파란 약 x" + std::to_string(blue), blue > 0 ? Color::Blue : Color::DarkGray);
    console.draw({x, y + 2}, "[2] 빨간 약 x" + std::to_string(red), red > 0 ? Color::Red : Color::DarkGray);
}

void HudView::drawBoss(Console& console, int y) const {
    const int x = m_origin.x;
    const Boss& boss = m_session.boss();
    if (m_session.progress().isComplete()) {
        return;
    }
    if (!boss.isActive()) {
        console.draw({x, y}, "사진이 절반쯤", Color::DarkGray);
        console.draw({x, y + 1}, "맞춰지면...", Color::DarkGray);
        return;
    }
    console.draw({x, y}, "보스", blink(500) ? Color::Red : Color::DarkRed);
    console.draw({x + 5, y}, boss.name(), Color::Red);
    console.draw({x, y + 1}, "(" + boss.symptom() + ")", Color::DarkRed);
    int line = y + 2;
    for (const auto& text : TextUtil::wrap(TextUtil::decode(boss.statusText()), kWidthColumns)) {
        console.draw({x, line++}, TextUtil::encode(text), Color::Yellow);
    }
}
