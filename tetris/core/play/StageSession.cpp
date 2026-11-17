#include "play/StageSession.h"
#include <array>
#include "common/GameConfig.h"

namespace {
// 회전할 때 벽이나 블록에 막히면 옆으로 살짝 밀어 본다 (wall kick)
constexpr std::array<Point, 6> kKicks{{{0, 0}, {-1, 0}, {1, 0}, {0, -1}, {-2, 0}, {2, 0}}};
}  // namespace

StageSession::StageSession(const StageConfig& config, ISoundPlayer& sound, std::mt19937& rng)
    : m_config(config),
      m_rng(rng),
      m_generator(rng, config::kPillBlockChancePercent),
      m_progress(m_bus),
      m_speed(config.baseDropMs),
      m_boss(config.makeBoss()),
      m_inventory(config::kPillLimitPerType),
      m_soundCue(sound) {
    m_bus.subscribe(m_speed);
    m_bus.subscribe(m_boss);
    m_bus.subscribe(m_inventory);
    m_bus.subscribe(m_soundCue);
    spawnNext();
}

void StageSession::subscribe(IGameEventListener& listener) {
    m_bus.subscribe(listener);
}

PlayContext StageSession::context() {
    return PlayContext{m_board, m_current, m_speed, m_progress, m_bus, m_rng};
}

void StageSession::reset() {
    m_board.reset();
    m_generator.reset();
    m_progress.reset();
    m_speed.reset(m_config.baseDropMs);
    m_boss = m_config.makeBoss();  // 같은 객체 자리에 새 보스를 넣으므로 구독은 그대로 유지된다
    m_inventory.clear();
    m_flashRows.clear();
    m_dropTimerMs = 0;
    m_state = State::Falling;
    spawnNext();
}

void StageSession::handle(Command command) {
    if (m_state != State::Falling) {
        return;
    }
    switch (command) {
        case Command::MoveLeft: move({-1, 0}); break;
        case Command::MoveRight: move({1, 0}); break;
        case Command::Rotate: rotate(); break;
        case Command::SoftDrop: stepDown(); m_dropTimerMs = 0; break;
        case Command::HardDrop: hardDrop(); break;
        case Command::UseBluePill: usePill(PillType::Blue); break;
        case Command::UseRedPill: usePill(PillType::Red); break;
        default: break;
    }
}

void StageSession::update(int dtMs) {
    if (m_state == State::Cleared || m_state == State::Failed) {
        return;
    }
    m_speed.update(dtMs);
    PlayContext ctx = context();
    m_boss.update(ctx, dtMs);

    if (m_state == State::LineFlash) {
        m_flashLeftMs -= dtMs;
        if (m_flashLeftMs <= 0) {
            finishLineClear();
        }
        return;
    }

    m_dropTimerMs += dtMs;
    while (m_state == State::Falling && m_dropTimerMs >= m_speed.dropIntervalMs()) {
        m_dropTimerMs -= m_speed.dropIntervalMs();
        stepDown();
    }
}

bool StageSession::canMove(Point delta) const {
    return m_board.canPlace(m_current, delta);
}

void StageSession::move(Point delta) {
    if (canMove(delta)) {
        m_current = m_current.moved(delta);
    }
}

std::optional<Tetromino> StageSession::findRotation() const {
    const Tetromino rotated = m_current.rotated();
    for (Point kick : kKicks) {
        if (m_board.canPlace(rotated, kick)) {
            return rotated.moved(kick);
        }
    }
    return std::nullopt;
}

void StageSession::rotate() {
    if (const auto rotated = findRotation()) {
        m_current = *rotated;
    }
}

void StageSession::stepDown() {
    if (canMove({0, 1})) {
        move({0, 1});
    } else {
        lockCurrent();
    }
}

void StageSession::hardDrop() {
    while (canMove({0, 1})) {
        move({0, 1});
    }
    lockCurrent();
}

Tetromino StageSession::ghost() const {
    Tetromino shadow = m_current;
    while (m_board.canPlace(shadow, {0, 1})) {
        shadow = shadow.moved({0, 1});
    }
    return shadow;
}

void StageSession::lockCurrent() {
    if (!m_board.canLock(m_current)) {
        fail();  // 블록이 보드 위로 삐져나옴
        return;
    }
    m_board.lock(m_current);
    m_bus.publish(GameEvent::Type::PieceLocked);
    m_dropTimerMs = 0;

    m_flashRows = m_board.findFullLines();
    if (m_flashRows.empty()) {
        spawnNext();
        return;
    }
    m_state = State::LineFlash;
    m_flashLeftMs = config::kLineFlashMs;
    m_bus.publish(GameEvent{GameEvent::Type::LinesCleared, static_cast<int>(m_flashRows.size()), m_flashRows.front()});
}

void StageSession::finishLineClear() {
    const std::vector<PillType> pills = m_board.pillsInRows(m_flashRows);
    const int count = static_cast<int>(m_flashRows.size());
    m_board.clearRows(m_flashRows);
    m_flashRows.clear();
    for (PillType pill : pills) {
        if (m_inventory.canAdd(pill)) {
            m_inventory.add(pill);
            m_bus.publish(GameEvent::Type::PillGained, static_cast<int>(pill));
        }
    }
    m_state = State::Falling;
    m_progress.addLines(count);
    if (m_progress.isComplete()) {
        PlayContext ctx = context();
        m_boss.update(ctx, 0);  // 보스 퇴장 처리 (가림막·표시 해제)
        m_state = State::Cleared;
        return;
    }
    spawnNext();
}

void StageSession::spawnNext() {
    m_current = m_generator.nextPiece();
    m_generator.advance();
    PlayContext ctx = context();
    m_boss.onPieceSpawn(ctx);
    if (!m_board.canPlace(m_current)) {
        fail();
    }
}

void StageSession::usePill(PillType type) {
    PlayContext ctx = context();
    if (!m_inventory.canUse(type, ctx)) {
        return;
    }
    m_inventory.use(type, ctx);
    // 줄이 내려오면서 떨어지던 블록과 겹치면 위로 올려 준다.
    while (!m_board.canPlace(m_current) && m_current.position().y > -Tetromino::kCellCount) {
        m_current = m_current.moved({0, -1});
    }
    if (m_progress.isComplete()) {
        m_boss.update(ctx, 0);
        m_state = State::Cleared;
    }
}

void StageSession::fail() {
    m_state = State::Failed;
    m_bus.publish(GameEvent::Type::StageFailed);
}
