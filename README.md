# ui — Finished screens and what each teammate must do (Owner B: Ssok)

Every picture in the game (opening, 12 rounds, album, house, ending) is already in `tetris/core/ui/`.
All art is drawn with code or hand-typed pixel maps only. No image files are loaded (professor's rule).
This document lists **what ui has ready** and **what each owner still has to do**.

---

## 1. Game flow (as drawn)

```
[House opening]  PrologueView.start(0)
  The old man takes his medicine on the living-room sofa and falls asleep -> wakes up as a child
  -> his daughter says "We're heading out for a bit" and leaves
  -> candy (medicine) in the kitchen -> album in the library
[Album]    AlbumView.open(0, -1) -> pick a photo (round)
[Stage 1]  Lake / Kitchen / Beach / Bedroom (boss)       -> album -> PrologueView.start(1) (sandwich)
[Stage 2]  Classroom / Soccer / Uni / Interview (boss)   -> album -> PrologueView.start(2) (burger)
[Stage 3]  Office / Wedding / Nursery / Today (boss)     -> last album page
[Ending]   EndingView.start()
  He puts the album down -> his daughter and son come in: "Dad, how do you feel now?"
  -> "...Better. Much better." -> THE END
```

## 2. What ui has ready

| Group | Classes | Notes |
|---|---|---|
| Play screen | `StageArtView` | Draws the round by reading `StageSession`. While `wantsPause()` is true a cutscene is playing, so stop the falling block |
| Rounds (Stage 1) | `LakeRound`, `KitchenRound`, `BeachRound`, `BedroomRound` | Bedroom is the boss |
| Rounds (Stage 2) | `ClassroomRound`, `SoccerRound`, `UniRound`, `InterviewRound` | Interview is the boss |
| Rounds (Stage 3) | `OfficeRound`, `WeddingRound`, `NurseryRound`, `FadingRound` | Today (FadingRound) is the final boss |
| Album | `AlbumView`, `AlbumScreen` | 3 pages, 12 slots. When a round is cleared, its memory item flies into the album |
| House | `PrologueView`, `HouseScene`, `HouseRooms` | `start(0)` opening, `start(1)` sandwich, `start(2)` burger |
| Ending | `EndingView` | The final scene with his children in the library |
| Menu | `MenuView` | Esc during play: RESUME, MEMORY BOOK, SETTINGS, QUIT TO TITLE, drawn over the dimmed game |
| Settings | `SettingsView` | Sound ON/OFF switch and the full list of controls |
| Memory Book | `MemoryBookView` | Progress of the whole run: all 12 memories, which are remembered, which round is being played now |
| Shared art | `RoundScene` (round base class), `LivingRoom`, `MenuArt.h`, `Stage2UI.h`, `Stage3Art.h` | |

### Lines each round's art uses (`goal()`)

| Round | Lines | Round | Lines | Round | Lines |
|---|---|---|---|---|---|
| Lake | 10 | Classroom | 10 | Office | 10 |
| Kitchen | 10 | Soccer | 10 | Wedding | 10 |
| Beach | 10 | Uni | 12 | Nursery | 10 |
| Bedroom (boss) | 20 | Interview (boss) | 20 | Today (boss) | 25 (part 1: 10 + part 2: 15) |

`StageArtView` scales the real number of cleared lines to these steps. If the real line counts match this table, the art moves exactly one step per line.

### How to use

```cpp
// House
prologue.start(stage);           // 0: opening, 1: sandwich, 2: burger
prologue.handle(command); prologue.update(dtMs); prologue.draw(console);
if (prologue.finished()) { /* open the album */ }

// Album
album.open(stage, clearedRound); // first time: clearedRound = -1
if (album.hasPick()) { int round = album.takePick(); }   // stage * 4 + slot
if (album.stageFinished()) { /* next house scene or the ending */ }

// Menu (call open() right after the game screen was drawn; it copies that picture as the background)
menu.open(art.frame());
menu.handle(command); menu.update(dtMs); menu.draw(console);
switch (menu.choice()) {
    case MenuView::Choice::Resume:      /* back to the game (menu is already closed) */ break;
    case MenuView::Choice::MemoryBook:  book.open(remembered, currentSlot, art.frame()); menu.clearChoice(); break;
    case MenuView::Choice::Settings:    settings.open(soundOn, art.frame()); menu.clearChoice(); break;
    case MenuView::Choice::QuitToTitle: /* go to the title */ break;
    default: break;
}
// Settings: when it closes, read settings.soundOn() and swap BeepSoundPlayer / MuteSoundPlayer
settings.handle(command); if (!settings.isOpen()) { soundOn = settings.soundOn(); }
// Memory Book: remembered = 12 bools (stage * 4 + slot), currentSlot = round being played or -1
book.handle(command); if (!book.isOpen()) { /* back to the menu */ }

// Ending
ending.start();
ending.handle(command); ending.update(dtMs); ending.draw(console);
if (ending.finished()) { /* game over */ }
```

---

## 3. What each teammate must do

### D. Joeunseo (scenes, flow, integration) — `flow/*`, `common/GameConfig.h`

The scenes in `flow/` do not use the ui Views yet. Connect them like this so the art shows up in the game.

1. `PrologueScene` -> use `PrologueView`. The game starts with `start(0)` (the old-man opening is already inside it; there is no separate opening to connect).
2. `AlbumScene` -> use `AlbumView`. Pass the round picked with `takePick()` to the play scene.
3. `PlayScene` -> use `StageArtView`. Subscribe it with `session.subscribe(art)` and stop the falling block while `wantsPause()` is true.
4. When a stage ends (`album.stageFinished()`), open `PrologueView.start(1)`; after Stage 2, open `start(2)`.
5. After the Stage 3 album, show `EndingView` instead of the current text ending (`EndingScene`).
6. Set the per-round line counts in `GameConfig.h` to match the table in section 2 (right now every round uses `kLinesPerPiece = 3`).
7. Menu: in `PlayScene`'s `Paused` phase, show `MenuView` instead of the current pause text and act on `choice()` (see the example in section 2). Esc on the menu means RESUME.
8. Settings: keep one `soundOn` value for the whole game. When `SettingsView` closes, swap the sound player between `BeepSoundPlayer` and `MuteSoundPlayer` (both already exist; the concrete objects are made in `main.cpp`).
9. Memory Book: keep which of the 12 rounds are cleared for the whole run (the album already tracks the same thing) and pass it to `MemoryBookView::open()` together with the round being played.

### C. Taegyeong Lee (stages, growth, bosses) — `stage/*`, `boss/*`, `medicine/*`

The art acts as if the rules below exist, but **the real board does not change yet**. These rules need to be made.

1. **Bosses appear only in round 4 of each stage**: Bedroom (Stage 1), Interview (Stage 2), Today (Stage 3). Register them in `StageCatalog` that way.
2. Interview (Stage 2 boss): odd-shaped blocks (`ShapeShiftGimmick`), ending with the screen turning white.
3. Today (Stage 3 boss), part 1, busy adult life (10 lines):
   - Lines 2, 5, 8: **Bills** — clear the bottom 2 rows with no points; the rows above drop down (`Board::clearRow()` twice).
   - Lines 1, 7, 10: **Faster** — blocks fall faster (`SpeedController`). Shredded paper piles up as 1x1 blocks; right now this is only drawn, so pieces pass through it. To make them real 1x1 blocks, use a single-cell function from A.
   - Line 4: **Deadline**; lines 3, 6, 9: **Years go by** (art only, no rule).
4. Today, part 2, old man (15 lines):
   - From line 4: **Forgetting** — stacked blocks disappear one by one and never come back (`Board::setHidden`, `ForgetGimmick`). Right now ui only hides them in the picture.
   - Line 8: **Blur** (no rule needed if the art is enough).
   - Line 12: **Static** — the board looks frozen but the block keeps falling (handled by the art, no rule).
5. Pills: connect the pill counter in the art (`Stage2UI::pills`) to the counts in `Inventory`.

### A. Hanbi Lim (Tetris core) — `block/*`, `board/*`

1. (Only if C makes the 1x1 paper blocks real) `Board` needs a function that places a single cell, e.g. `void addCell(Point pos, TetrominoType type)`.
2. Otherwise, ui needs no changes in the core.

### Everyone — shared files

1. Add the new classes to `docs/class_diagram.drawio` (in the `ui` group): the rounds `ClassroomRound`, `SoccerRound`, `UniRound`, `InterviewRound`, `OfficeRound`, `WeddingRound`, `NurseryRound`, `FadingRound` (inheriting `RoundScene`), and the views `EndingView`, `MenuView`, `SettingsView`, `MemoryBookView` (inheriting `View`), plus `LivingRoom`.
2. It is a shared file, so open a PR and get everyone's approval.

