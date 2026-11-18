#pragma once
#include <string>
#include <vector>
#include "ui/TextBoxView.h"
#include "flow/Scene.h"

// 대사 한 줄
struct NarrativeLine {
    std::string speaker;  // 빈 문자열이면 내레이션
    std::string text;
};

// 자막을 한 줄씩 넘기는 장면의 공통 뼈대 (프롤로그, 엔딩).
//     "다음 줄로 넘기기"는 여기서 처리하고, 배경 그림과 끝났을 때 할 일만 자식이 정한다.
class NarrativeScene : public Scene {
public:
    NarrativeScene(GameServices& services, std::vector<NarrativeLine> lines);

    void onEnter() override;
    void handle(Command command) override;
    void update(int dtMs) override;
    void render(Console& console) const override;

protected:
    virtual void renderBackground(Console& console) const = 0;
    virtual void onNarrationEnd() = 0;
    virtual void onLineShown(std::size_t index);
    virtual void onUpdate(int dtMs);
    virtual bool canAdvance() const;

    std::size_t lineIndex() const { return m_index; }
    int timeInLineMs() const { return m_lineMs; }

    TextBoxView m_textBox;

private:
    void showLine(std::size_t index);

    std::vector<NarrativeLine> m_lines;
    std::size_t m_index = 0;
    int m_lineMs = 0;
    bool m_finished = false;
};
