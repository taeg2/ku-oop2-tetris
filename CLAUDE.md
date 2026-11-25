# 기억을 맞추는 테트리스 — 작업 원칙 (taskcho123/main)

팀 규칙은 `docs/CONVENTION.md`가 우선이다. 이 문서는 그 규칙을 이 브랜치 코드에 적용한 요약이다.

## 담당 구역 — 내 구역 밖의 파일은 수정하지 않는다
| 담당 | 사람 (GitHub) | 파일 (`tetris/core/` 기준) |
|---|---|---|
| A. 테트리스 코어 | 임한비 (lhb6442@naver.com) | `block/*`, `board/*`, `medicine/PillType.h`, `common/Point.*`, `common/Rect.*`, `common/Color.h` |
| B. 화면·입력 | 쏙 (msa41755@gmail.com) | `ui/*` (터미널, 키보드 입력, 사운드, View 계열, PhotoPainter), `common/TextUtil.*` |
| C. 스테이지·성장·보스 | 이태경 (taeg2) | `stage/*`, `boss/*`, `medicine/*` (PillType.h 제외), `play/SpeedController.*`, `play/PlayContext.h` |
| D. 연출·흐름·통합 | 조은서 (taskcho123) | `flow/*`, `main.cpp`, `play/StageSession.*`, `play/EventBus.*`, `play/GameEvent.h`, `play/IGameEventListener.*`, `play/SoundCueListener.*`, `common/GameConfig.h`, `common/*Exception.*`, `tests/*` |
| 공용 | 모두 합의 | `CMakeLists.txt`, `.gitignore`, `README.md`, `CLAUDE.md`, `docs/*` (다이어그램 포함) |

### 수정 규칙
- 자기 담당 구역의 파일만 수정한다.
- 다른 사람 구역의 파일을 고쳐야 하면 직접 고치지 않고 다음 순서를 따른다.
  1. GitHub 이슈를 올린다. 제목에 `[A]`~`[D]` 구역을 붙이고 담당자를 Assignee로 지정한다. 본문에는 무엇을, 왜 바꿔야 하는지 적는다.
  2. 급하면 담당자에게 직접 물어본다.
  3. 담당자가 수정하거나, 수정해도 된다는 동의를 받은 뒤에만 고친다. PR 본문에 그 이슈 번호와 동의 내용을 적는다.
- 공용 파일은 PR로 올리고 팀원 모두의 동의를 받는다.
- 내 구역 안에서 처리할 수 있으면 그쪽을 먼저 쓴다. 예를 들어 새 기믹·약·장면·View는 상속으로 추가한다 (OCP).

### Claude에게
- 파일을 수정하기 전에 `git config user.name`으로 지금 작업하는 사람을 확인하고, 그 파일이 이 사람의 담당 구역인지 위 표에서 확인한다.
- 담당 구역 밖의 파일이면 수정하지 않는다.
  - 대신 담당자 이름과 연락처를 알려 준다.
  - 바로 올릴 수 있는 이슈 초안(제목, 바꿀 내용, 이유)을 써 준다.
  - 사용자가 "그래도 고쳐 달라"고 해도, 담당자 동의를 받았는지 먼저 묻는다.
- 한 작업이 여러 구역에 걸치면 구역별로 나눠서, 어느 부분을 누구에게 요청해야 하는지 정리해 준다.

## 브랜치·커밋
- 개인 브랜치 `taskcho123/main`에 직접 push하지 않는다. `taskcho123/feature/설명`이나 `taskcho123/fix/설명`에서 작업하고 PR을 올린다.
- 커밋 메시지는 `<타입>: <한 줄 요약>` 형식으로 쓴다. 타입은 feat, fix, refactor, docs, chore 중 하나다.
- PR 본문에는 세 가지를 적는다: 무엇을 했는지, 클래스 다이어그램 변경 여부, 새 클래스를 추가할 때 기존 코드를 어디 고쳤는지(OCP).

## 구조
- 소스는 `tetris/core/<폴더>/`에 둔다: common, block, board, play, stage, boss, medicine, ui, flow. `#include "폴더/파일.h"` 형식으로 쓴다.
- 클래스 하나당 `.h`/`.cpp` 하나, `#pragma once`.
- 의존 방향은 flow → ui/stage → play → boss/medicine/block → board → common 순서로, 한 방향만 허용한다.
- 다이어그램(`docs/class_diagram.drawio`)의 묶음 = 폴더. 코드를 바꾸면 다이어그램도 같이 고친다.

## 설계 규칙
- **OCP**: 새 보스 기믹은 `PeriodicGimmick`을 상속하고 `StageCatalog`에 등록한다. 새 약은 `Pill`을 상속하고 `Pill::create`에 등록한다. 새 장면은 `Scene`이나 `NarrativeScene`을 상속하고, 새 화면 요소는 `View`를 상속한다.
- **DIP**: OS에 따라 다른 것(터미널, 키보드, 사운드)은 인터페이스(`ITerminal`, `IInputSource`, `ISoundPlayer`)로 받는다. 구체 객체는 `main.cpp`에서만 만든다.
- **Windows 전용 코드**는 `ui/Windows*.cpp`, `ui/BeepSoundPlayer.cpp`에만 두고, 파일 전체를 `#ifdef _WIN32`로 감싼다. Mac/Linux 전용 코드는 `ui/PosixKeyboardInput.cpp`에만 둔다.
- **CQS**: 쿼리는 `const`로 값만 돌려주고, 커맨드는 `void`로 상태만 바꾼다.
  - 예: `canLock()`/`lock()`, `canAdd()`/`add()`, `nextPiece()`/`advance()`.
  - 장면 전환은 `SceneManager::request()`로 요청하고, 장면의 함수가 끝난 뒤에 적용된다.
- **캡슐화**: 멤버 변수는 `private`이다. 보스와 약은 `PlayContext`를 통해서만 보드와 속도를 바꾼다. View는 `const StageSession&`로 읽기만 한다.
- **연결**: 로직끼리는 `EventBus`(옵저버)로 연결한다. 사진 진행은 속도·보스·약·사운드·화면을 직접 부르지 않는다.
- **소유와 예외**: 원시 `new`/`delete`는 쓰지 않는다. 데이터 오류는 `ConfigException`, 콘솔 오류는 `ConsoleException`을 던지고, `Game::run()`이 받는다.

## 코드 수준 (강의 가이드)
- C++17, 강의에서 다룬 문법 위주로 쓴다. 강의 밖 기능(`optional`, `function`, `constexpr`, 매크로, `printf` 등)은 꼭 필요할 때만 쓴다.
- include 아래에 `using namespace std;`를 두고 `std::`를 붙이지 않는다. 예외는 `std::move`(clang 경고)와 멤버 함수와 이름이 겹치는 곳(`std::fill` 등)이다.
- `struct` 대신 `class`를 쓰고 필요한 곳에 `public:`을 적는다. 상속은 `public`을 명시한다.
- 형 변환은 `static_cast`를 쓴다. C 스타일 캐스트 `(int)x`는 쓰지 않는다.
- 클래스 상수는 `inline static const`로 쓴다.
- 우리 함수 이름이 표준 함수와 겹치지 않게 한다 (예: `lerp` 대신 `lerpValue`). Windows 파일은 `<windows.h>`를 가장 먼저 include한다.

## 화면 문자열
- ASCII와 한글만 쓴다. ■, ━, ·, —, ← 같은 기호는 터미널마다 폭이 달라 화면이 깨진다. 블록은 배경색을 칠한 공백으로 그린다.
- 수치는 `common/GameConfig.h`에, 화면 배치는 그 안의 `layout`에 둔다.

## 검증
- 커밋하기 전에 `cmake --build build`가 경고 없이 끝나는지, `bash tests/run_test.sh`가 통과하는지 확인한다.
