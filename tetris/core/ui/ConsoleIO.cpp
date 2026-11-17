#include "ui/ConsoleIO.h"

#include <algorithm>
#include <iostream>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#endif

namespace {
    // ANSI 이스케이프 코드
    const std::string reset = "\x1b[0m";
    const std::string gray = "\x1b[90m";
    const std::string bold = "\x1b[1m";
    const std::string yellowText = "\x1b[33m";
    const std::string clearLineEnd = "\x1b[K";

    const std::string emptyCell = gray + ". " + reset;
    const std::string hiddenCell = "\x1b[100m  " + reset;     // 암전된 칸
    const std::string photoFill = "\x1b[47m";                // 채워진 사진 조각 배경
    const std::string defaultBlockColor = "\x1b[47m";

    // 화살표 키는 일반 문자와 겹치지 않게 따로 번호를 붙인다
    const int keyUp = 1000;
    const int keyDown = 1001;
    const int keyLeft = 1002;
    const int keyRight = 1003;
    const int keyEscape = 27;

    // 가운데 정렬 기준 화면 폭
    const int screenWidth = 80;

    // 보드 왼쪽에서 오른쪽 정보창까지의 간격
    const std::string panelGap = "   ";
}

ConsoleIO::ConsoleIO()
    : frame(),
      actions(),
      blockColors({
          { 'I', "\x1b[46m" },            // 하늘색
          { 'O', "\x1b[43m" },            // 노란색
          { 'T', "\x1b[45m" },            // 보라색
          { 'S', "\x1b[42m" },            // 초록색
          { 'Z', "\x1b[41m" },            // 빨간색
          { 'J', "\x1b[44m" },            // 파란색
          { 'L', "\x1b[48;5;208m" },      // 주황색
      }),
      keyActions({
          { keyLeft, Action::MoveLeft },   { 'a', Action::MoveLeft },     { 'A', Action::MoveLeft },
          { keyRight, Action::MoveRight }, { 'd', Action::MoveRight },    { 'D', Action::MoveRight },
          { keyDown, Action::SoftDrop },   { 's', Action::SoftDrop },     { 'S', Action::SoftDrop },
          { keyUp, Action::Rotate },       { 'w', Action::Rotate },       { 'W', Action::Rotate },
          { 'x', Action::Rotate },         { 'X', Action::Rotate },
          { ' ', Action::HardDrop },
          { '1', Action::UseMedicine1 },   { '2', Action::UseMedicine2 },
          { '\r', Action::Confirm },       { '\n', Action::Confirm },
          { 'q', Action::Quit },           { 'Q', Action::Quit },         { keyEscape, Action::Quit },
      }),
#ifdef _WIN32
      originalOutputMode(0)
#else
      originalTerminal()
#endif
{
    setUpConsole();
}

ConsoleIO::~ConsoleIO() {
    restoreConsole();
}

// ===== InputSource =====

std::vector<Action> ConsoleIO::getActions() const {
    return actions;
}

void ConsoleIO::poll() {
    actions.clear();
    readPendingKeys();
}

// ===== Renderer =====

void ConsoleIO::beginFrame() {
    frame.clear();
}

void ConsoleIO::drawPlay(const PlayField& field, const StageSession& session) {
    const std::vector<std::string> boardLines = makeBoardLines(field, session.getStage().getBoss());
    const std::vector<std::string> panelLines = makePanelLines(field, session);
    const int boardWidth = field.getBoard().getWidth() * 2 + 2;
    const size_t lineCount = std::max(boardLines.size(), panelLines.size());

    for (size_t i = 0; i < lineCount; ++i) {
        std::string line = (i < boardLines.size()) ? boardLines[i] : std::string(boardWidth, ' ');
        if (i < panelLines.size()) {
            line += panelGap + panelLines[i];
        }
        addLine(line);
    }

    addLine("");
    if (session.isCleared()) {
        addLine(bold + yellowText + " 사진이 완성되었습니다!  [Enter] 앨범에 넣기" + reset);
    }
    else if (session.isFailed()) {
        addLine(bold + yellowText + " 이 시기의 기억을 다시 찾아볼까요?  [Enter] 다시 하기  [Q] 그만하기" + reset);
    }
    else {
        addLine(gray + " ← → 이동   ↑ 회전   ↓ 내리기   Space 바로 떨어뜨리기   1 2 약 사용   Q 그만하기" + reset);
    }
}

void ConsoleIO::drawStory(const std::vector<std::string>& lines) {
    addLine("");
    addLine("");
    addLine("");
    for (const auto& text : lines) {
        addLine(makeCentered(text));
        addLine("");
    }
    addLine("");
    addLine(gray + makeCentered("[Enter]") + reset);
}

void ConsoleIO::drawAlbum(const Album& album, int newPhotoIndex) {
    addLine("");
    addLine(bold + makeCentered("~ 가족 앨범 ~") + reset);
    addLine("");
    for (int i = 0; i < album.getPhotoCount(); ++i) {
        const Stage& stage = album.getPhoto(i);
        std::string title = "  [" + std::to_string(i + 1) + "장] STAGE " + std::to_string(stage.getNumber())
            + " · " + stage.getTitle();
        if (i == newPhotoIndex) {
            title = bold + yellowText + title + "   ← 새로 들어온 사진" + reset;
        }
        addLine(title);
        addLine("      \"" + stage.getAlbumCaption() + "\"");
        addLine("");
    }
    addLine(gray + makeCentered("[Enter]") + reset);
}

void ConsoleIO::endFrame() {
    // 커서를 맨 위로 옮겨 덮어쓰고, 남은 아래쪽 내용은 지운다
    std::cout << "\x1b[H" << frame << "\x1b[J" << std::flush;
}

// ===== 화면 문자열 만들기 =====

std::vector<std::string> ConsoleIO::makeBoardLines(const PlayField& field, const Boss& boss) const {
    const Board& board = field.getBoard();
    const std::vector<Point> pieceCells = field.isGameOver() ? std::vector<Point>() : field.getCurrentPiece().getCells();
    const char pieceSymbol = field.getCurrentPiece().getSymbol();
    const std::string border = "+" + std::string(board.getWidth() * 2, '-') + "+";

    std::vector<std::string> lines;
    lines.push_back(border);
    for (int row = 0; row < board.getHeight(); ++row) {
        std::string line = "|";
        for (int col = 0; col < board.getWidth(); ++col) {
            const Point cell(row, col);
            if (boss.hidesCell(cell)) {
                line += hiddenCell;
            }
            else if (std::find(pieceCells.begin(), pieceCells.end(), cell) != pieceCells.end()) {
                line += makeCell(pieceSymbol);
            }
            else if (board.isEmpty(cell)) {
                line += emptyCell;
            }
            else {
                line += makeCell(board.getSymbolAt(cell));
            }
        }
        lines.push_back(line + "|");
    }
    lines.push_back(border);
    return lines;
}

std::vector<std::string> ConsoleIO::makePanelLines(const PlayField& field, const StageSession& session) const {
    const Stage& stage = session.getStage();
    const PhotoProgress& progress = session.getProgress();
    const FallSpeed& speed = field.getSpeed();
    const Boss& boss = stage.getBoss();
    const MedicineInventory& medicines = session.getMedicines();

    std::vector<std::string> lines;
    lines.push_back(bold + "STAGE " + std::to_string(stage.getNumber()) + "  " + stage.getTitle() + reset);
    lines.push_back(gray + "블록 테마: " + stage.getBlockTheme() + reset);
    lines.push_back("");

    // 4분할 사진 틀
    for (const auto& line : makePhotoFrameLines(progress)) {
        lines.push_back(line);
    }
    if (progress.isComplete()) {
        lines.push_back("사진 4/4 완성");
    }
    else {
        lines.push_back("사진 " + std::to_string(progress.getFilledPieceCount()) + "/" + std::to_string(progress.getPieceCount())
            + "  (다음 조각까지 " + std::to_string(progress.getLinesUntilNextPiece()) + "줄)");
    }
    for (int i = 0; i < progress.getFilledPieceCount(); ++i) {
        lines.push_back(gray + "  " + std::to_string(i + 1) + ". " + stage.getPhotoPart(i) + reset);
    }
    lines.push_back("");

    // 속도
    std::string speedLine = "속도 " + std::to_string(speed.getLevel()) + "단계";
    if (speed.getSlowRemainingMs() > 0) {
        speedLine += "  (느려짐 " + formatSeconds(speed.getSlowRemainingMs()) + ")";
    }
    lines.push_back(speedLine);
    lines.push_back("");

    // 다음 블록
    lines.push_back("다음 블록");
    for (const auto& line : makeNextPieceLines(field.getNextPiece())) {
        lines.push_back("  " + line);
    }
    lines.push_back("");

    // 약
    std::string medicineLine = "약  ";
    if (medicines.getCount() == 0) {
        medicineLine += gray + "없음" + reset;
    }
    for (int i = 0; i < medicines.getCount(); ++i) {
        medicineLine += "[" + std::to_string(i + 1) + "] " + medicines.getName(i) + "  ";
    }
    lines.push_back(medicineLine);
    lines.push_back("");

    // 보스
    if (boss.isActive()) {
        lines.push_back(bold + yellowText + "보스: " + boss.getName() + reset);
        lines.push_back("  다음 방해까지 " + formatSeconds(boss.getMsUntilNextGimmick()));
    }
    return lines;
}

std::vector<std::string> ConsoleIO::makePhotoFrameLines(const PhotoProgress& progress) const {
    const std::string blank(6, ' ');
    const std::string fill = photoFill + blank + reset;
    std::vector<std::string> lines;

    // 사진이 완성되면 4분할 경계선이 사라지고 한 장으로 합쳐진다
    if (progress.isComplete()) {
        const std::string wideFill = photoFill + std::string(13, ' ') + reset;
        lines.push_back("+-------------+");
        for (int i = 0; i < 5; ++i) {
            lines.push_back("|" + wideFill + "|");
        }
        lines.push_back("+-------------+");
        return lines;
    }

    // 조각 순서: 1 왼쪽 위, 2 오른쪽 위, 3 왼쪽 아래, 4 오른쪽 아래
    const int filled = progress.getFilledPieceCount();
    const std::string border = "+------+------+";
    for (int half = 0; half < 2; ++half) {
        const std::string left = (filled > half * 2) ? fill : blank;
        const std::string right = (filled > half * 2 + 1) ? fill : blank;
        lines.push_back(border);
        lines.push_back("|" + left + "|" + right + "|");
        lines.push_back("|" + left + "|" + right + "|");
    }
    lines.push_back(border);
    return lines;
}

std::vector<std::string> ConsoleIO::makeNextPieceLines(const Tetromino& piece) const {
    const std::vector<Point> cells = piece.getCells();
    const Point origin = piece.getPosition();

    int topRow = piece.getBoxSize();
    int bottomRow = -1;
    for (const auto& cell : cells) {
        topRow = std::min(topRow, cell.getRow() - origin.getRow());
        bottomRow = std::max(bottomRow, cell.getRow() - origin.getRow());
    }

    std::vector<std::string> lines;
    for (int row = topRow; row <= bottomRow; ++row) {
        std::string line;
        for (int col = 0; col < piece.getBoxSize(); ++col) {
            const Point cell = origin.translated(row, col);
            const bool filled = std::find(cells.begin(), cells.end(), cell) != cells.end();
            line += filled ? makeCell(piece.getSymbol()) : "  ";
        }
        lines.push_back(line);
    }
    return lines;
}

std::string ConsoleIO::makeCell(char symbol) const {
    const auto found = blockColors.find(symbol);
    const std::string color = (found != blockColors.end()) ? found->second : defaultBlockColor;
    return color + "  " + reset;
}

std::string ConsoleIO::makeCentered(const std::string& text) const {
    const int padding = std::max(0, (screenWidth - getDisplayWidth(text)) / 2);
    return std::string(padding, ' ') + text;
}

int ConsoleIO::getDisplayWidth(const std::string& text) const {
    // UTF-8 첫 바이트로 글자 수를 센다. 3바이트 이상 글자(한글 등)는 화면에서 2칸을 차지한다.
    int width = 0;
    for (unsigned char c : text) {
        if ((c & 0x80) == 0 || (c & 0xE0) == 0xC0) {
            width += 1;
        }
        else if ((c & 0xF0) == 0xE0 || (c & 0xF8) == 0xF0) {
            width += 2;
        }
    }
    return width;
}

std::string ConsoleIO::formatSeconds(int ms) const {
    return std::to_string(ms / 1000) + "." + std::to_string((ms % 1000) / 100) + "초";
}

// ===== 커맨드 =====

void ConsoleIO::addLine(const std::string& line) {
    frame += line + clearLineEnd + "\n";
}

void ConsoleIO::addKey(int key) {
    const auto found = keyActions.find(key);
    if (found != keyActions.end()) {
        actions.push_back(found->second);
    }
}

// ===== OS마다 다른 부분 =====

#ifdef _WIN32

void ConsoleIO::readPendingKeys() {
    while (_kbhit()) {
        const int key = _getch();
        if (key == 0 || key == 224) {
            // 화살표 같은 특수 키는 두 번에 나뉘어 들어온다
            const int extendedKey = _getch();
            if (extendedKey == 72) addKey(keyUp);
            if (extendedKey == 80) addKey(keyDown);
            if (extendedKey == 75) addKey(keyLeft);
            if (extendedKey == 77) addKey(keyRight);
        }
        else {
            addKey(key);
        }
    }
}

void ConsoleIO::setUpConsole() {
    SetConsoleOutputCP(CP_UTF8);
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(output, &mode)) {
        originalOutputMode = mode;
        SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    std::cout << "\x1b[?25l\x1b[2J" << std::flush;   // 커서 숨기기, 화면 지우기
}

void ConsoleIO::restoreConsole() {
    std::cout << reset << "\x1b[?25h\n" << std::flush;  // 색 되돌리기, 커서 보이기
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleMode(output, originalOutputMode);
}

#else

void ConsoleIO::readPendingKeys() {
    unsigned char buffer[64];
    const ssize_t count = read(STDIN_FILENO, buffer, sizeof(buffer));
    for (ssize_t i = 0; i < count; ++i) {
        // 화살표 키는 ESC [ A~D 세 바이트로 들어온다
        if (buffer[i] == keyEscape && i + 2 < count && buffer[i + 1] == '[') {
            const unsigned char code = buffer[i + 2];
            if (code == 'A') addKey(keyUp);
            if (code == 'B') addKey(keyDown);
            if (code == 'C') addKey(keyRight);
            if (code == 'D') addKey(keyLeft);
            i += 2;
        }
        else {
            addKey(buffer[i]);
        }
    }
}

void ConsoleIO::setUpConsole() {
    // 엔터를 기다리지 않고, 누른 키를 화면에 찍지 않고, 입력이 없으면 바로 돌아오게 한다
    tcgetattr(STDIN_FILENO, &originalTerminal);
    termios raw = originalTerminal;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    std::cout << "\x1b[?25l\x1b[2J" << std::flush;   // 커서 숨기기, 화면 지우기
}

void ConsoleIO::restoreConsole() {
    std::cout << reset << "\x1b[?25h\n" << std::flush;  // 색 되돌리기, 커서 보이기
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTerminal);
}

#endif
