# ku-oop2-tetris
[C++]로 구현한 클래식 테트리스 게임입니다. 객체 지향 설계를 학습하기 위해 제작되었습니다.

## 기억을 맞추는 테트리스 (taskcho123 브랜치)

알츠하이머로 기억을 잃어 가는 아버지의 가족 앨범을, 줄을 지워 한 장씩 되찾는 콘솔 스토리 테트리스입니다.
기획은 [docs/기획안.md](docs/기획안.md), 클래스 구조는 [docs/class_diagram.drawio](docs/class_diagram.drawio)(app.diagrams.net에서 열기)를 보면 됩니다.

### 빌드와 실행

화면은 100 x 30 칸입니다. 창이 작으면 늘려 주세요.
집·라운드·앨범·메뉴·엔딩 그림은 **Sixel 이미지**로 그리므로 Sixel을 지원하는 터미널에서 실행해야 보입니다.

| OS | 그림이 보이는 터미널 | 안 보이는 터미널 |
|---|---|---|
| Windows | Windows Terminal 1.22 이상 | 예전 콘솔 창(conhost), Visual Studio 디버그 콘솔 |
| macOS | iTerm2, WezTerm | 기본 "터미널" 앱 |

#### Windows (PowerShell, Windows Terminal에서)

준비: Visual Studio Installer에서 **C++를 사용한 데스크톱 개발** 워크로드를 설치합니다 (CMake 포함). 아래 명령은 **Developer PowerShell for VS**에서 실행합니다.

```powershell
cd ku-oop2-tetris
cmake -S . -B build
cmake --build build --config Debug
.\build\Debug\core.exe            # 효과음 끄기: .\build\Debug\core.exe --mute
```

- Visual Studio로 실행하려면 **파일 → 열기 → 폴더**로 `ku-oop2-tetris`를 열고 시작 항목에서 `core.exe`를 고릅니다. 다만 그림을 보려면 빌드된 `core.exe`를 Windows Terminal에서 실행하세요.
- 효과음(Beep)은 Windows에서만 나옵니다.

#### macOS (iTerm2 또는 WezTerm에서)

준비 (처음 한 번):

```bash
xcode-select --install
brew install cmake
```

빌드와 실행:

```bash
cd ku-oop2-tetris
cmake -S . -B build
cmake --build build
./build/core
```

#### 최신 코드 받기

```bash
git fetch origin
git switch feat/ui-merge
git pull
```

### 조작

| 키 | 동작 |
|---|---|
| ← → (A D) | 이동 |
| ↑ (W) | 회전 |
| ↓ (S) | 한 칸 내리기 |
| Space | 바로 떨어뜨리기, 대사 넘기기 |
| 방향키 | 집 장면에서 걷기, 앨범·메뉴에서 고르기 |
| 1 / 2 | 파란 약(속도 감소) / 빨간 약(맨 아래 줄 삭제) |
| Enter | 확인 |
| Esc | 메뉴 열기/닫기 |
| Q | 타이틀로 / 종료 |

### 게임 흐름

타이틀 → 집 (오프닝) → 앨범에서 라운드 고르기 → 플레이 → 앨범 (스테이지마다 4라운드) → 집 → … → 엔딩 → 타이틀

- STAGE 1 어린 시절 (호수, 부엌, 해변, 침실), STAGE 2 진학과 취업 (교실, 축구, 대학, 면접), STAGE 3 결혼과 출산 (첫 직장, 결혼식, 아기방, 오늘)
- 줄을 3개 지울 때마다 사진 조각(라운드) 하나가 끝나고, 블록이 빨라집니다. 라운드 끝 연출을 본 뒤 앨범에 기억이 붙습니다.
- 사진이 2/4가 되면 보스가 나타납니다.
  - 짙은 안개: 보드 일부를 가림
  - 엉킨 실타래: 블록 모양을 바꿈
  - 지우개: 쌓인 칸을 지움
- 사진이 4/4가 되면 보스가 물러가고, 앨범 페이지가 넘어가며 다음 집 장면으로 갑니다.
- 플레이 중 Esc를 누르면 메뉴가 열립니다: 계속하기, 기억의 책, 설정 (소리 켜기/끄기), 타이틀로.
- 실패하면 지금 라운드부터 다시 할 수 있습니다.

### 폴더 (tetris/core)

| 폴더 | 내용 |
|---|---|
| `main.cpp` | OS에 맞는 터미널·키보드·사운드 구체 객체를 만들어 `Game`에 주입 |
| `common/` | Point, Rect, TextUtil(한글 폭), 예외 계층, GameConfig(모든 수치) |
| `block/` | Tetromino, BlockGenerator(7-bag) |
| `board/` | Board, Cell |
| `play/` | StageSession(한 판 규칙), SpeedController, EventBus(옵저버), PlayContext |
| `stage/` | StageCatalog(스테이지 데이터), PhotoArt(도트 사진), PhotoProgress, Album |
| `boss/` | Boss, BossGimmick → PeriodicGimmick → Blackout / ShapeShift / Forget |
| `medicine/` | Pill → BluePill / RedPill, Inventory |
| `ui/` | Console(화면 버퍼), ITerminal(Windows/ANSI), 키보드 입력(Windows/Posix), 사운드, View 계열 |
| `flow/` | Game(메인 루프), SceneManager, Scene 계열(타이틀·집·플레이·앨범·게임오버·엔딩), StoryArt(장면 사이에 이어지는 그림과 진행), SwitchableSound |

### 테스트

봇이 세 스테이지와 12라운드를 끝까지 플레이하며 사진 진행, 보스 기믹, 약, 장면 전환(앨범, 메뉴, 기억의 책, 설정, 엔딩)을 확인합니다.
주요 장면 화면은 `tests/test_screens/`에 저장됩니다 (글자 화면은 `.txt`, 그림은 `.ppm`). 테스트용 `CaptureTerminal`이 `ITerminal`을 대신합니다.

macOS / Linux:

```bash
bash tests/run_test.sh
```

Windows: `run_test.sh`는 bash 스크립트라 **WSL**(Ubuntu)에서 저장소 폴더로 들어가 실행합니다. 처음 한 번 `sudo apt install g++`이 필요합니다.

```bash
bash tests/run_test.sh
```
