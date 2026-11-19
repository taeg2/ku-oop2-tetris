# ui — Stage 1 화면 (담당 B: 쏙)

Stage 1 플레이 화면, 라운드를 고르는 앨범, 집 오프닝을 그리는 View들이다.
그림은 모두 코드와 직접 입력한 픽셀 맵으로만 그렸다. 이미지 파일은 불러오지 않는다.

## 화면 출력

- 그림 한 장은 480 x 270 픽셀(`Frame`), 색은 프레임마다 최대 256색이다.
- `SixelEncoder`가 2배로 키워 Sixel로 바꾸고, `Console::drawImage()`로 넘긴다.
- `Console::present()`가 글자 버퍼를 출력한 뒤 `ITerminal::writeRaw()`로 이미지를 내보낸다.
- Sixel을 지원하는 터미널이 필요하다 (Windows Terminal 1.22 이상).
- 애니메이션은 100ms에 한 프레임이다 (`kFrameMs`).

## View 3개

| View | 쓰는 장면 | 하는 일 |
|---|---|---|
| `StageArtView` | PlayScene | 라운드 4개(호수, 부엌, 해변, 침실)를 StageSession을 읽어서 그린다 |
| `AlbumView` | AlbumScene | 걸린 사진으로 다음 라운드를 고른다. 고르기 전에는 나갈 수 없다 |
| `PrologueView` | PrologueScene | 집 오프닝. 음식을 먹고 서재에서 앨범을 집으면 끝난다 |

### StageArtView

```cpp
StageArtView art({0, 0}, session, config::kLinesPerPiece);
session.subscribe(art);
art.update(dtMs);
if (!art.wantsPause()) { /* 블록 진행 */ }
art.draw(console);
```

- `LinesCleared` 이벤트를 받아 그림을 진행한다. 실제 줄 수를 라운드의 그림 칸 수(`goal()`, 보통 10, 침실은 20)로 비율 환산한다.
- `PieceFilled` 이벤트를 받으면 끝 연출(페이드)이 끝난 뒤 다음 라운드로 넘어간다.
- `wantsPause()`가 true인 동안은 연출 중이므로 블록을 멈춘다.

### AlbumView

```cpp
album.open(0, clearedRound);
album.handle(command);
album.update(dtMs);
if (album.hasPick()) { int round = album.takePick(); }
if (album.stageFinished()) { /* Stage 2 오프닝으로 */ }
album.draw(console);
```

- `open(stage, clearedRound)`: 처음 열 때 `clearedRound`는 -1이다.
- `takePick()`은 `stage * 4 + 칸` 번호를 돌려준다.
- Stage 1의 네 칸이 다 차면 4초 뒤 앨범이 페이지를 넘기고 "Eat something first..." 쪽지를 보여 준 뒤 `stageFinished()`가 true가 된다.

| 키 | 동작 |
|---|---|
| 좌우 | 사진 고르기 |
| Enter / Space | 사진 크게 보기, CANCEL 또는 PLAY 누르기 |
| 위 / 아래 | CANCEL / PLAY 선택 |
| Esc | 크게 보기 닫기 |

### PrologueView

```cpp
prologue.start(0);
prologue.handle(command);
prologue.update(dtMs);
if (prologue.finished()) { /* 앨범 열기 */ }
prologue.draw(console);
```

- `start(0)`: 아이가 주인공. 딸이 문 앞에서 인사하고 나가고, 부엌의 사탕을 먹으면 비틀거린다. 서재 의자 위의 앨범을 집는다.
- `start(1)`: Stage 1이 끝난 뒤. 중학생이 된 주인공이 부엌의 샌드위치를 먹고("tastes weird") 서재에서 앨범을 다시 집는다.
- 방향키로 걷고, Enter / Space로 대화를 넘기거나 음식과 앨범을 집는다.

## 파일

| 묶음 | 파일 |
|---|---|
| View | `StageArtView`, `AlbumView`, `PrologueView` |
| 라운드 | `RoundScene`(부모), `LakeRound`, `KitchenRound`, `BeachRound`, `BedroomRound`, `BedroomArt.h`, `BedroomPaint.h` |
| 앨범 | `AlbumScreen` |
| 집 | `HouseScene`, `HouseRooms` |
| 게임 상태 읽기 | `PlayStateReader`(인터페이스), `SessionPlayState`(StageSession 어댑터), `EmptyPlayState`(빈 보드) |
| 그리기 도구 | `PixelArt.h`, `PixelFont.h`, `ToyBlocks.h`, `People.h`, `SixelEncoder.h` |
| 출력 | `Console`, `ITerminal`, `AnsiTerminal`, `WindowsTerminal` (이미지 출력 함수 추가) |

## 새 라운드 추가 (OCP)

1. `RoundScene`을 상속하고 `drawBackground`, `drawNext`, `drawHud`를 구현한다.
2. 필요하면 `goal()`, `onLinesCleared()`, `endingDone()`, `playing()`을 재정의한다.
3. `StageArtView`와 `AlbumView` 생성자의 라운드 목록에 추가한다.

## 다른 구역이 할 일

- D (연출·흐름): 장면에서 위 View를 소유하고 그리기, EventBus 구독, 라운드 끝 -> 앨범 -> 다음 라운드 흐름.
- C (스테이지·보스): 라운드별 규칙 기믹 (침실 얼룩, 블록 빼앗기, 좌우 반전 등).
