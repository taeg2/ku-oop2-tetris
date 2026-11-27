#include "ui/SessionPlayState.h"
using namespace std;

SessionPlayState::SessionPlayState(const StageSession& session) : m_session(session) {}
int SessionPlayState::toArtType(TetrominoType type) {
    switch (type) {
        case TetrominoType::I: return 0;
        case TetrominoType::O: return 1;
        case TetrominoType::T: return 2;
        case TetrominoType::S: return 3;
        case TetrominoType::Z: return 4;
        case TetrominoType::J: return 5;
        case TetrominoType::L: return 6;
    }
    return 7;
}
TetrominoType SessionPlayState::fromArtType(int type) {
    static const TetrominoType kTypes[7] = { TetrominoType::I, TetrominoType::O, TetrominoType::T, TetrominoType::S, TetrominoType::Z, TetrominoType::J, TetrominoType::L };
    return kTypes[type < 0 || type > 6 ? 0 : type];
}
PlayStateReader::Cells SessionPlayState::toCells(const Tetromino::Cells& cells) {
    Cells out;
    for (const Point& p : cells) out.emplace_back(p.x, p.y);
    return out;
}
int SessionPlayState::at(int x, int y) const {
    const Cell& cell = m_session.board().cellAt(Point{x, y});
    if (!cell.isFilled()) return -1;
    for (int t = 0; t < kTetrominoTypeCount; ++t) {
        TetrominoType type = static_cast<TetrominoType>(t);
        if (Tetromino::colorOf(type) == cell.color()) return toArtType(type);
    }
    return 7;
}
int SessionPlayState::current() const { return toArtType(m_session.current().type()); }
PlayStateReader::Cells SessionPlayState::pieceCells() const { return toCells(m_session.current().cells()); }
PlayStateReader::Cells SessionPlayState::ghostCells() const { return toCells(m_session.ghost().cells()); }
PlayStateReader::Cells SessionPlayState::shapeOf(int type) { return toCells(Tetromino(fromArtType(type)).localCells()); }
PlayStateReader::Cells SessionPlayState::shape(int type) const { return shapeOf(type); }
int SessionPlayState::next() const { return toArtType(m_session.nextType()); }
const vector<int>& SessionPlayState::clearingRows() const { return m_session.flashingRows(); }
bool SessionPlayState::clearing() const { return !m_session.flashingRows().empty(); }
bool SessionPlayState::over() const { return m_session.isFailed(); }
bool SessionPlayState::hasPiece() const { return m_session.hasActivePiece(); }
int SessionPlayState::pillAt(int x, int y) const {
    const Cell& cell = m_session.board().cellAt(Point{x, y});
    if (!cell.isFilled() || !cell.hasPill()) return -1;
    return static_cast<int>(cell.pill());
}
int SessionPlayState::piecePill() const { return m_session.current().hasPill() ? static_cast<int>(m_session.current().pill()) : -1; }
int SessionPlayState::piecePillIndex() const { return m_session.current().pillCellIndex(); }
int SessionPlayState::pillCount(int kind) const { return m_session.inventory().count(kind == 0 ? PillType::Blue : PillType::Red); }
bool SessionPlayState::hidden(int x, int y) const { return m_session.board().isHidden(Point{x, y}); }
bool SessionPlayState::marked(int x, int y) const { return m_session.board().isMarked(Point{x, y}); }
