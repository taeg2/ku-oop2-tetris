#pragma once

// 블록이 한 칸 떨어지는 데 걸리는 시간(낙하 간격)을 관리한다.
// - 속도 단계(level)가 오를 때마다 낙하 간격이 일정하게 줄어든다. (사진 조각을 채울 때)
// - 감속 효과가 남아 있는 동안에는 낙하 간격이 몇 배로 길어진다. (파란 약)
// 수치는 기획안에서 아직 정해지지 않아 생성자에서 받는다.
class FallSpeed {
public:
    // startIntervalMs: 단계 0의 낙하 간격
    // stepMs: 단계가 하나 오를 때 줄어드는 간격
    // minIntervalMs: 아무리 빨라져도 이보다 짧아지지 않는 간격
    // slowMultiplier: 감속 중일 때 낙하 간격에 곱하는 수
    FallSpeed(int startIntervalMs = 800, int stepMs = 100, int minIntervalMs = 100, int slowMultiplier = 2);

    // ===== 쿼리 =====

    // 지금 적용되는 낙하 간격 (감속 효과 포함)
    int getFallIntervalMs() const;

    // 속도 단계 (0부터 시작)
    int getLevel() const;

    // 남은 감속 시간. 감속 중이 아니면 0이다.
    int getSlowRemainingMs() const;

    // ===== 커맨드 =====

    // 속도 단계를 하나 올린다.
    void speedUp();

    // durationMs 동안 감속한다. 이미 감속 중이면 남은 시간을 durationMs로 다시 맞춘다.
    void slowDown(int durationMs);

    // 시간이 elapsedMs만큼 흘렀음을 알린다. 감속 시간이 줄어든다.
    void update(int elapsedMs);

    // 단계 0, 감속 없음 상태로 되돌린다. (스테이지 시작·재도전)
    void reset();

private:
    int startIntervalMs;
    int stepMs;
    int minIntervalMs;
    int slowMultiplier;
    int level;
    int slowRemainingMs;
};
