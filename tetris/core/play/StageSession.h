#pragma once
#include <optional>
#include <random>
#include <vector>
#include "block/BlockGenerator.h"
#include "board/Board.h"
#include "block/Tetromino.h"
#include "boss/Boss.h"
#include "play/EventBus.h"
#include "medicine/Inventory.h"
#include "stage/PhotoProgress.h"
#include "play/SoundCueListener.h"
#include "play/SpeedController.h"
#include "stage/StageConfig.h"
#include "ui/Command.h"
#include "ui/ISoundPlayer.h"

// 스테이지 한 판의 규칙을 진행한다. 보드·블록·사진 진행·속도·보스·약을 소유하고
//     이벤트 버스로 서로 연결한다. 화면은 모르며, View에는 const 접근자만 제공한다.
class StageSession {
public:
    StageSession(const StageConfig& config, ISoundPlayer& sound, std::mt19937& rng);
    StageSession(const StageSession&) = delete;
    StageSession& operator=(const StageSession&) = delete;

    void subscribe(IGameEventListener& listener);
    void handle(Command command);
    void update(int dtMs);
    void reset();         // 재도전: 스테이지 시작 상태로
    void restartBoard();  // 재도전: 사진 진행·보스·약·속도는 그대로 두고 보드만 비워 지금 라운드부터 다시

    bool isCleared() const { return m_state == State::Cleared; }
    bool isFailed() const { return m_state == State::Failed; }
    bool hasActivePiece() const { return m_state == State::Falling; }

    const StageConfig& config() const { return m_config; }
    const Board& board() const { return m_board; }
    const Tetromino& current() const { return m_current; }
    Tetromino ghost() const;
    TetrominoType nextType() const { return m_generator.nextPiece().type(); }
    const PhotoProgress& progress() const { return m_progress; }
    const SpeedController& speed() const { return m_speed; }
    const Boss& boss() const { return m_boss; }
    const Inventory& inventory() const { return m_inventory; }
    const std::vector<int>& flashingRows() const { return m_flashRows; }

private:
    enum class State { Falling, LineFlash, Cleared, Failed };

    PlayContext context();
    bool canMove(Point delta) const;
    void move(Point delta);
    std::optional<Tetromino> findRotation() const;  // 벽 차기(wall kick)까지 고려한 회전 결과
    void rotate();
    void stepDown();
    void hardDrop();
    void lockCurrent();
    void finishLineClear();
    void spawnNext();
    void usePill(PillType type);
    void fail();

    const StageConfig& m_config;
    std::mt19937& m_rng;
    EventBus m_bus;
    Board m_board;
    BlockGenerator m_generator;
    Tetromino m_current;
    PhotoProgress m_progress;
    SpeedController m_speed;
    Boss m_boss;
    Inventory m_inventory;
    SoundCueListener m_soundCue;

    State m_state = State::Falling;
    int m_dropTimerMs = 0;
    int m_flashLeftMs = 0;
    std::vector<int> m_flashRows;
};
