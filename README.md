# ku-oop2-tetris
[C++]로 구현한 클래식 테트리스 게임입니다. 객체 지향 설계를 학습하기 위해 제작되었습니다.

## 기억을 맞추는 테트리스 (taskcho123 브랜치)

알츠하이머로 기억을 잃어 가는 아버지의 가족 앨범을, 줄을 지워 한 장씩 되찾는 콘솔 스토리 테트리스입니다.
기획은 [docs/기획안.md](docs/기획안.md), 클래스 구조는 [docs/class_diagram.drawio](docs/class_diagram.drawio)(app.diagrams.net에서 열기)를 보면 됩니다.

### 빌드와 실행

`docs/CONVENTION.md` 8번(빌드 환경)과 같습니다. Windows는 Visual Studio로 폴더를 열고, Mac은 VS Code + CMake로 빌드합니다.

```bash
cmake -S . -B build
cmake --build build
./build/core          # Windows: build\Debug\core.exe,  --mute 를 붙이면 효과음 끔
```

- 화면 크기는 100 x 30입니다. 창이 작으면 늘려 주세요.
- 효과음(Beep)은 Windows에서만 나옵니다.

### 조작

| 키 | 동작 |
|---|---|
| ← → (A D) | 이동 |
| ↑ (W) | 회전 |
| ↓ (S) | 한 칸 내리기 |
| Space | 바로 떨어뜨리기, 대사 넘기기 |
| 1 / 2 | 파란 약(속도 감소) / 빨간 약(맨 아래 줄 삭제) |
| Enter | 확인 |
| Esc | 일시정지, 이야기 건너뛰기 |
| Q | 타이틀로 / 종료 |

### 게임 흐름

타이틀 → 프롤로그 → STAGE 1 어린 시절 → 앨범 → STAGE 2 진학과 취업 → 앨범 → STAGE 3 결혼과 출산 → 앨범 → 엔딩

- 줄을 3개 지울 때마다 사진 조각이 하나 돌아오고, 블록이 빨라집니다.
- 사진이 2/4가 되면 보스가 나타납니다.
  - 짙은 안개: 보드 일부를 가림
  - 엉킨 실타래: 블록 모양을 바꿈
  - 지우개: 쌓인 칸을 지움
- 사진이 4/4가 되면 보스가 물러가고 사진이 앨범에 들어갑니다.
- 실패하면 그 스테이지를 처음부터 다시 할 수 있습니다.

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
| `flow/` | Game(메인 루프), SceneManager, Scene 계열(타이틀·프롤로그·플레이·앨범·게임오버·엔딩) |

### 테스트 (Mac/Linux)

```bash
bash tests/run_test.sh
```

봇이 세 스테이지를 끝까지 플레이하며 사진 진행, 보스 기믹, 약, 장면 전환을 확인합니다.
주요 장면 화면은 `tests/test_screens/`에 저장됩니다. 테스트용 `CaptureTerminal`이 `ITerminal`을 대신합니다.
