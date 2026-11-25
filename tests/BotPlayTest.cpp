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
#include "flow/PlayScene.h"
#include "flow/PrologueScene.h"
#include "flow/SceneManager.h"
#include "flow/StoryArt.h"
#include "flow/SwitchableSound.h"
#include "flow/TitleScene.h"
#include "ui/PixelArt.h"

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
    void writeRaw(Point, const std::string& bytes) override { ++images; imageBytes += bytes.size(); }  // Sixel 그림은 개수만 센다
    std::vector<Cell> screen = std::vector<Cell>(W * H);
    int images = 0;
    std::size_t imageBytes = 0;
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

// 그림 View가 방금 render()한 Frame을 PPM 이미지로 저장한다 (팔레트는 render() 직후에만 맞다)
static void dumpArt(const std::string& name, const Frame& f) {
    std::ofstream out("test_screens/" + name + ".ppm", std::ios::binary);
    out << "P6\n" << Frame::W << " " << Frame::H << "\n255\n";
    const auto& pal = palette();
    for (uint8_t c : f.px) {
        const RGB rgb = c < pal.size() ? pal[c] : RGB{0, 0, 0};
        out.put((char)rgb.r); out.put((char)rgb.g); out.put((char)rgb.b);
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
    MuteSoundPlayer mute;
    SwitchableSound sound(mute);
    StageCatalog catalog;
    Album album(config::kStageCount);
    StoryArt story(catalog, sound, rng);
    SceneManager scenes;
    GameServices services{sound, album, story, catalog, rng, scenes};
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

    // --- 장면 흐름: 타이틀 -> 집 오프닝 -> (앨범 -> 플레이) x 12 라운드 -> 집 -> ... -> 엔딩 ---
    // 그림(Sixel) 만들기는 느리므로 봇이 플레이하는 동안에는 renderEvery 프레임마다 한 번만 그린다
    int renderEvery = 1, frameNo = 0;
    auto frame = [&](int n, std::optional<Command> cmd = std::nullopt) {
        for (int i = 0; i < n; ++i) {
            if (cmd && i == 0) scenes.handle(*cmd);
            scenes.update(16);
            if (!scenes.isRunning()) return;
            if (++frameNo % renderEvery == 0) { console.clear(); scenes.render(console); console.present(); }
        }
    };
    scenes.change(std::make_unique<TitleScene>(services));
    frame(10); dump("scene_title");
    CHECK(g_terminal.images == 0);  // 타이틀은 글자 화면
    frame(5, Command::Confirm); frame(120);
    CHECK(g_terminal.images > 0);   // 집 오프닝은 그림(Sixel)으로 그린다
    story.house().render(); dumpArt("art_house_opening", story.house().frame());
    for (int i = 0; i < 40; ++i) frame(20, Command::Confirm);  // 대사 넘기기
    story.house().render(); dumpArt("art_house_talk", story.house().frame());

    // 집 안을 걸어 다니는 건 봇이 하기 어려우므로, 스테이지마다 앨범부터 시작한다
    int stuck = 0, failures = 0;
    renderEvery = 60;
    bool menuChecked = false, dumpedPlay = false;
    for (int stage = 0; stage < 3; ++stage) {
        scenes.change(std::make_unique<AlbumScene>(services, stage, AlbumScene::kNoRoundCleared));
        frame(30); story.album().render(); dumpArt("art_album_stage" + std::to_string(stage + 1), story.album().frame());
        int frames = 0;
        while (frames++ < 100000) {
            // 플레이 장면인지: 세션이 블록을 떨어뜨리는 중이고, 앨범이 닫혀 있고, 라운드 끝 처리가 남지 않았다
            const bool inPlay = story.hasSession() && story.stageIndex() == stage && story.session().hasActivePiece() &&
                                !story.album().isOpen() && !story.roundEndPending();
            if (stage == 2 && story.ending().finished()) break;
            if (stage < 2 && story.house().stage() == stage + 1) break;  // 다음 집 장면이 열렸다
            if (story.hasSession() && story.session().isFailed()) { ++failures; frame(100); frame(5, Command::Confirm); continue; }
            if (inPlay && !story.stageArt().wantsPause()) {
                if (!dumpedPlay && story.session().board().filledCells().size() > 20) {
                    dumpedPlay = true;
                    story.stageArt().render(); dumpArt("art_play_lake", story.stageArt().frame());
                }
                if (!menuChecked && story.session().progress().pieces() == 1) {
                    // 메뉴 -> 기억의 책 -> 닫기 -> 설정에서 소리 끄기 -> 닫기 -> 계속하기
                    menuChecked = true;
                    frame(5, Command::Pause); story.menu().render(); dumpArt("art_menu", story.menu().frame());
                    CHECK(story.menu().isOpen());
                    frame(5, Command::SoftDrop); frame(5, Command::Confirm);
                    CHECK(story.book().isOpen()); story.book().render(); dumpArt("art_memory_book", story.book().frame());
                    CHECK(story.book().rememberedCount() == 1);
                    frame(5, Command::Pause); CHECK(!story.book().isOpen());
                    frame(5, Command::SoftDrop); frame(5, Command::Confirm);
                    CHECK(story.settings().isOpen()); frame(5, Command::Confirm); CHECK(!sound.isOn());
                    story.settings().render(); dumpArt("art_settings", story.settings().frame());
                    frame(5, Command::Confirm); CHECK(sound.isOn());
                    frame(5, Command::Pause); CHECK(!story.settings().isOpen());
                    frame(5, Command::Pause); CHECK(!story.menu().isOpen());
                    continue;
                }
                if (frames % 14 == 0) { frame(1, botCommand(story.session(), stuck)); continue; }
            } else if (!inPlay && frames % 30 == 0) {
                frame(1, Command::Confirm);  // 앨범에서 다음 라운드 고르기
                continue;
            }
            frame(1);
        }
        int count = 0;
        for (int slot = 0; slot < (stage + 1) * StoryArt::kRoundsPerStage; ++slot) count += story.remembered()[(std::size_t)slot];
        std::printf("scene stage %d: frames=%d remembered=%d failures=%d\n", stage + 1, frames, count, failures);
        CHECK(count == (stage + 1) * StoryArt::kRoundsPerStage);
        CHECK(story.session().isCleared());
        if (stage < 2) { frame(30); story.house().render(); dumpArt("art_house_stage" + std::to_string(stage + 2), story.house().frame()); }
    }
    renderEvery = 1;
    CHECK(story.ending().finished());
    CHECK(menuChecked);
    story.ending().render(); dumpArt("art_ending", story.ending().frame());
    CHECK(album.isComplete());
    frame(5, Command::Confirm);  // 엔딩 -> 타이틀
    CHECK(scenes.isRunning());
    std::printf("scene flow ok, images=%d\n", g_terminal.images);
    return 0;
}
