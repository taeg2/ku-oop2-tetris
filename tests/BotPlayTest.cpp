#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include "common/TextUtil.h"
#include "ui/ITerminal.h"
#include "board/Board.h"
#include "stage/Album.h"
#include "common/GameConfig.h"
#include "stage/StageCatalog.h"
#include "play/StageSession.h"
#include "ui/MuteSoundPlayer.h"
#include "ui/BoardView.h"
#include "ui/HudView.h"
#include "ui/NextBlockView.h"
#include "ui/PhotoFrameView.h"
#include "flow/AlbumScene.h"
#include "flow/EndingScene.h"
#include "flow/SceneManager.h"
#include "flow/TitleScene.h"

#define CHECK(c) do { if (!(c)) { std::printf("CHECK FAILED %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

// 화면 출력을 메모리에 받아 두는 테스트용 터미널 (ITerminal 구현을 바꿔 끼운 예)
class CaptureTerminal : public ITerminal {
public:
    struct Cell { char32_t ch = U' '; int fg = 7; int bg = 0; bool cont = false; };
    static constexpr int W = 100, H = 30;
    void open(int, int) override {}
    void close() override {}
    void clearScreen() override { for (auto& c : screen) c = Cell{}; }
    void write(Point pos, const std::u32string& text, Color fg, Color bg) override {
        int x = pos.x;
        for (char32_t ch : text) {
            const int w = TextUtil::charWidth(ch);
            if (x >= 0 && x < W && pos.y >= 0 && pos.y < H) screen[pos.y * W + x] = {ch, (int)fg, (int)bg, false};
            if (w == 2 && x + 1 < W) screen[pos.y * W + x + 1] = {U' ', (int)fg, (int)bg, true};
            x += w;
        }
    }
    void flush() override {}
    std::vector<Cell> screen = std::vector<Cell>(W * H);
};
static CaptureTerminal g_terminal;

static void dump(const std::string& name) {
    std::ofstream out("test_screens/" + name + ".txt");
    std::ofstream attr("test_screens/" + name + ".attr");
    for (int y = 0; y < CaptureTerminal::H; ++y) {
        std::u32string line;
        for (int x = 0; x < CaptureTerminal::W; ++x) {
            const auto& c = g_terminal.screen[y * CaptureTerminal::W + x];
            if (!c.cont) line.push_back(c.ch);
            char buf[16]; std::snprintf(buf, 16, "%05x:%02x ", c.cont ? 0u : (unsigned)c.ch, c.fg | (c.bg << 4)); attr << buf;
        }
        attr << "\n";
        out << TextUtil::encode(line) << "\n";
    }
}

struct Counter : IGameEventListener { std::map<int,int> n; void onEvent(const GameEvent& e) override { n[(int)e.type]++; } };
// ---------- 간단한 봇 ----------
struct Plan { int rot; int x; };
static double evaluate(const Board& b, int lines) {
    int heights[Board::kWidth] = {}; int holes = 0;
    for (int x = 0; x < Board::kWidth; ++x) {
        bool seen = false;
        for (int y = 0; y < Board::kHeight; ++y) {
            bool f = b.cellAt({x, y}).isFilled();
            if (f && !seen) { seen = true; heights[x] = Board::kHeight - y; }
            else if (!f && seen) holes++;
        }
    }
    int agg = 0, bump = 0;
    for (int x = 0; x < Board::kWidth; ++x) { agg += heights[x]; if (x) bump += std::abs(heights[x] - heights[x - 1]); }
    return -0.51 * agg + 0.76 * lines - 0.36 * holes - 0.18 * bump;
}
static Plan plan(const StageSession& s) {
    const Tetromino& cur = s.current();
    double best = -1e9; Plan bp{cur.rotation(), cur.position().x};
    for (int r = 0; r < 4; ++r) {
        Tetromino rot = cur.rotated((r - cur.rotation() + 4) % 4);
        for (int x = -3; x < Board::kWidth + 1; ++x) {
            Tetromino p = rot.moved({x - rot.position().x, 0});
            if (!s.board().canPlace(p)) continue;
            while (s.board().canPlace(p, {0, 1})) p = p.moved({0, 1});
            Board copy = s.board();
            if (!copy.canLock(p)) continue;
            copy.lock(p);
            auto rows = copy.findFullLines();
            copy.clearRows(rows);
            double v = evaluate(copy, (int)rows.size());
            if (v > best) { best = v; bp = {r, x}; }
        }
    }
    return bp;
}
static Command botCommand(const StageSession& s, int& stuck) {
    Plan p = plan(s);
    const Tetromino& cur = s.current();
    if (++stuck > 12) { stuck = 0; return Command::HardDrop; }
    if (cur.rotation() != p.rot) return Command::Rotate;
    if (cur.position().x < p.x) return Command::MoveRight;
    if (cur.position().x > p.x) return Command::MoveLeft;
    stuck = 0;
    return Command::HardDrop;
}

int main() {
    std::mt19937 rng(12345);
    MuteSoundPlayer sound;
    StageCatalog catalog;
    Album album(config::kStageCount);
    SceneManager scenes;
    GameServices services{sound, album, catalog, rng, scenes};
    Console console(g_terminal, config::kScreenWidth, config::kScreenHeight);

    // --- 단위 확인 ---
    for (int t = 0; t < kTetrominoTypeCount; ++t) {
        Tetromino a(static_cast<TetrominoType>(t), {3, 3});
        auto c0 = a.cells(); auto c4 = a.rotated(4).cells();
        CHECK(c0 == c4);
    }
    {
        Board b;
        for (int x = 0; x < Board::kWidth; ++x) { Tetromino o(TetrominoType::O, {0, 0}); (void)o; }
        CHECK(b.filledCells().empty());
        Tetromino i(TetrominoType::I, {0, 17});  // 가로 I: y=18
        CHECK(b.canLock(i)); b.lock(i); b.lock(i.moved({4, 0}));
        Tetromino o(TetrominoType::O, {8, 17}); b.lock(o);  // (8..9, 17..18)
        auto rows = b.findFullLines(); CHECK(rows.size() == 1 && rows[0] == 18);
        b.clearRows(rows);
        CHECK(b.cellAt({8, 18}).isFilled() && !b.cellAt({0, 18}).isFilled());
        bool threw = false; try { b.cellAt({-1, 0}); } catch (const std::out_of_range&) { threw = true; } CHECK(threw);
        CHECK(b.isRowEmpty(19)); b.clearRow(18); CHECK(b.filledCells().empty());
        CHECK(!b.canLock(Tetromino(TetrominoType::I, {0, -2})));
    }
    {
        BlockGenerator g(rng, 0); std::map<int, int> counts;
        for (int i = 0; i < 70; ++i) { counts[(int)g.nextPiece().type()]++; g.advance(); }
        for (auto& [k, v] : counts) CHECK(v == 10);
    }
    std::printf("unit checks ok\n");

    // --- 봇으로 3 스테이지 플레이 ---
    for (int stage = 0; stage < 3; ++stage) {
        StageSession s(catalog.get(stage), sound, rng);
        Counter counter; s.subscribe(counter);
        BoardView bv({layout::kBoardX, layout::kBoardY}, s);
        HudView hv({layout::kHudX, layout::kHudY}, s);
        NextBlockView nv({layout::kNextX, layout::kNextY}, s);
        PhotoFrameView pv({layout::kPhotoX, layout::kPhotoY}, s);
        int frames = 0, stuck = 0, attempts = 1; bool dumpedBoss = false, dumpedEffect = false;
        int pillsUsed = 0, actions = 0;
        while (!s.isCleared() && frames < 200000) {
            ++frames;
            if (s.isFailed()) { s.reset(); attempts++; continue; }
            if (s.hasActivePiece() && frames % 14 == 0) {
                // 약도 써 본다
                ++actions;
                if (s.inventory().count(PillType::Red) > 0 && actions % 50 == 0) { s.handle(Command::UseRedPill); pillsUsed++; }
                else if (s.inventory().count(PillType::Blue) > 0 && actions % 70 == 0) { s.handle(Command::UseBluePill); pillsUsed++; }
                else s.handle(botCommand(s, stuck));
            }
            s.update(16);
            bv.update(16); pv.update(16); hv.update(16);
            bool effect = s.board().isHidden({0,0}) ;
            for (int y = 0; y < Board::kHeight && !effect; ++y) for (int x = 0; x < Board::kWidth; ++x)
                if (s.board().isHidden({x, y}) || s.board().isMarked({x, y})) effect = true;
            if ((s.boss().isActive() && !dumpedBoss && s.board().filledCells().size() > 25) || (effect && !dumpedEffect)) {
                console.clear(); bv.draw(console); nv.draw(console); hv.draw(console); pv.draw(console); console.present();
                dump("stage" + std::to_string(stage + 1) + (effect ? "_effect" : "_boss"));
                if (effect) dumpedEffect = true; else dumpedBoss = true;
            }
        }
        std::printf("stage %d: cleared=%d frames=%d attempts=%d pieces=%d pillsUsed=%d bossActive=%d\n", stage + 1,
                    s.isCleared(), frames, attempts, s.progress().pieces(), pillsUsed, s.boss().isActive());
        CHECK(s.isCleared());
        CHECK(pillsUsed > 0);
        std::printf("   bossTriggers=%d pillsGained=%d lines=%d\n", counter.n[(int)GameEvent::Type::BossTriggered], counter.n[(int)GameEvent::Type::PillGained], counter.n[(int)GameEvent::Type::LinesCleared]);
        CHECK(counter.n[(int)GameEvent::Type::BossTriggered] > 0);
        CHECK(!s.boss().isActive());
        for (int i = 0; i < 100; ++i) pv.update(16);
        console.clear(); bv.draw(console); nv.draw(console); hv.draw(console); pv.draw(console); console.present();
        dump("stage" + std::to_string(stage + 1) + "_cleared");
    }

    // --- 장면 흐름: 타이틀 -> 프롤로그 -> 플레이 (무작위 입력) ---
    auto frame = [&](int n, std::optional<Command> cmd = std::nullopt) {
        for (int i = 0; i < n; ++i) {
            if (cmd && i == 0) scenes.handle(*cmd);
            scenes.update(16);
            if (!scenes.isRunning()) return;
            console.clear(); scenes.render(console); console.present();
        }
    };
    scenes.change(std::make_unique<TitleScene>(services));
    frame(10); dump("scene_title");
    frame(5, Command::Confirm); frame(120); dump("scene_prologue_0");
    for (int i = 0; i < 3; ++i) { frame(5, Command::Confirm); frame(5, Command::Confirm); }
    frame(200); dump("scene_prologue_3");
    for (int i = 0; i < 6; ++i) { frame(5, Command::Confirm); frame(5, Command::Confirm); }
    frame(30); dump("scene_play_intro");
    frame(150); dump("scene_play_start");
    std::uniform_int_distribution<int> pick(0, 6);
    const Command cmds[] = {Command::MoveLeft, Command::MoveRight, Command::Rotate, Command::SoftDrop, Command::HardDrop, Command::UseBluePill, Command::UseRedPill};
    for (int i = 0; i < 4000 && scenes.isRunning(); ++i) {
        frame(1, cmds[pick(rng)]);
    }
    frame(1); dump("scene_random_play");
    frame(5, Command::Pause); dump("scene_paused");
    frame(5, Command::Pause);
    for (int i = 0; i < 3000 && scenes.isRunning(); ++i) frame(1, Command::HardDrop);
    dump("scene_after_drops");

    // --- 앨범과 엔딩 ---
    album.clear(); album.add(catalog.get(0).photo); album.add(catalog.get(1).photo);
    scenes.change(std::make_unique<AlbumScene>(services, 1));
    frame(60); dump("album_showcase");
    frame(130); dump("album_insert");
    frame(200); dump("album_caption");
    album.add(catalog.get(2).photo);
    scenes.change(std::make_unique<EndingScene>(services));
    frame(150); dump("ending_0");
    for (int i = 0; i < 4; ++i) { frame(5, Command::Confirm); frame(5, Command::Confirm); }
    frame(40); dump("ending_reassemble_mid");
    frame(400); dump("ending_reassemble_done");
    for (int i = 0; i < 6; ++i) { frame(5, Command::Confirm); frame(200, Command::Confirm); }
    dump("ending_last");
    for (int i = 0; i < 3; ++i) { frame(5, Command::Confirm); }
    std::printf("scene flow ok, running=%d\n", scenes.isRunning());
    return 0;
}
