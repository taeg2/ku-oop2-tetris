# STAGE 1 screen (UI) prototype: four rounds and the boss fight

This is a standalone prototype of the screen side of Stage 1 (childhood, the lake house).
The game rules (blocks, board, line clears, bosses, pills) belong to the team's code. This folder only decides what the game looks like.
Use it as a reference when you build the other stages or connect it to the team's code.

## 1. At a glance

| Round | Name | Place | Goal | NEXT holder | HUD | Each cleared line | Ending |
|---|---|---|---|---|---|---|---|
| 1 | Breeze | The lake | 10 lines = THE BOAT | The angel teddy's towel | Chalkboard | 4 blocks fly to the kid in the boat, +1 chalk tally. Every 4 lines a gust blows the towel away | Mom calls him in for dinner, then fade to black |
| 2 | Dinner | Lake house kitchen | 10 lines = FISH SOUP | Polka-dot pot (the worm shakes the handle) | Sticky note on the aquarium | 3 blocks fly up into the tank and become a fish, the kid takes a spoonful | The worm tips the last piece out of the pot, they leave, the bear-skin rug gets up and turns off the stove, then fade |
| 3 | Sand Castles | The beach at sunset | 10 lines = 5 sand castles | An octopus's tentacles hold the piece up | Upside-down bucket (one starfish per line) | The kid pats the sand 4 times. 2 lines = 1 castle | "HONEY! TIME FOR BED!", he waves to the octopus and runs off, the castles leak black goo, then fade |
| 4 | Bedtime (boss) | His bedroom at night | 20 lines | None (the goo swallows the book) | Toy soldiers, one per line | See section 5 | Parents come in and talk to him, he walks out with them, then fade |

Memory items for the Memory Book: Round 1 the boat, Round 2 the fish soup, Round 3 his bucket and shovel.

## 2. Files

| File | What it does |
|---|---|
| `main.cpp`, `Game.*` | Start-up, main loop (10 frames per second), the round list, key handling |
| `Scene.*` | Parent class of every round, plus shared UI (MENU/SETTING signs, Memory Book) |
| `LakeRound.*`, `KitchenRound.*`, `BeachRound.*`, `BedroomRound.*` | Rounds 1 to 4 |
| `BedroomPaint.h`, `BedroomArt.h` | Round 4's closet, werewolf, bed, quilt, kid and lamp, drawn with code (werewolf only appears in the closet gimmick) |
| `People.h` | Kid, mom and dad as hand-typed sprites (head, body and legs parts) |
| `Art.h`, `Font.h` | The 480x270 frame, palette, drawing functions, 3x5 font |
| `Tetris.h` | A simple stand-in Tetris for the prototype only (not needed once it is connected to the team's code) |
| `ToyBlocks.h` | How the blocks are drawn |
| `Renderer.h`, `SixelEncoder.h` | Turns the picture into Sixel and sends it to the terminal |
| `Console.*` | Windows Terminal input and output, keyboard |
| `tools/sim.cpp` | A bot that plays a round to the end on Linux, used to dump frames for preview videos |

## 3. Art rules

- No image files are loaded, and there is no converter.
- Characters and small objects are hand-typed sprites. One character is one color and `.` is transparent, for example `"..OHHSS.."`.
- Big things (rooms, the lake, sand castles, the closet, the bed) are drawn with code: shapes, lines and fills.
- The frame is 480x270 with at most 256 colors per frame (the Sixel limit). The palette is rebuilt every frame with `resetPalette()`.
- The display scale is picked automatically from the terminal size (up to 4x).

## 4. How a frame is drawn

`Scene::draw` calls these from back to front:

`drawBackground` -> `drawBoardFrame` -> blocks -> flashing rows -> `drawForeground` -> `drawNext` -> `drawHud` -> `drawEffects`

Functions a round overrides:

| Function | Meaning |
|---|---|
| `update(g, t)` | Advance the animations each frame |
| `onLinesCleared(rows, total, t)` | The moment lines are cleared |
| `endingDone()` | The ending has finished (the next round starts after this) |
| `playing(t)` | If false, input and gravity stop (used by intros) |
| `control(g, t)` | Rounds that change the rules (Round 4) touch the block state here |
| `swapLeftRight()` | Swap the left and right keys |

## 5. The boss fight (Round 4)

The round ends at 20 cleared lines. Each line adds one toy soldier.

**Intro.** Mom says goodnight and leaves, lightning flashes, the book on the shelf (the NEXT holder) flies off and the goo swallows it, then the goo crawls in through the window and covers the top of the board. There is no NEXT for the rest of the round. Play starts about 18 seconds in (`PLAY0`).

**Three gimmicks**

| Lines | What happens | Rule effect |
|---|---|---|
| 3 | An eye flies in and lands in his hoodie | LEFT and RIGHT are swapped |
| 7 | The werewolf leans out of the closet and stains appear on the board | Stained cells are blocked, and a piece that lands on one is eaten and does not place. Lasts about 7 seconds |
| 9 | Tentacles come out of the chest and from under the bed | They snatch the falling piece away |

**Ending.** The werewolf is not part of the ending: the closet is simply closed when it starts. The door shakes 3 times, the tentacles slither back and the goo dries. The parents come in and say "ARE YOU OKAY?" and "COME SLEEP WITH US." The kid crawls out from under the quilt, hops over the foot of the bed, and walks out with them, then the screen fades to black.

Where it lives: `drawIntro`, `drawCloset`, `drawChest`, `drawHoodie`, `drawStains`, `drawTentacles` and `drawEnding` in `BedroomRound.cpp`. The timing constants are at the top of `BedroomRound.h`.

## 6. Connecting it to the team's code

The scenes read the Tetris state in only a handful of places, so one adapter with the same function names can let them read the team's `StageSession` instead.

| Prototype | Team's code |
|---|---|
| `g.next()` | `StageSession::nextType()` |
| `g.at(x,y)` | cells of `board()` |
| `g.current()`, `px()`, `py()`, `rotation()` | `current()` |
| `g.ghostY()` | `ghost()` |
| `g.clearing()`, `clearingRows()` | `flashingRows()` |
| `g.over()` | `isFailed()` |
| `g.lines()` | `progress()` (the team counts pieces, not lines) |
| `onLinesCleared` | `GameEvent::LinesCleared` |
| Round finished | `GameEvent::PieceFilled` |
| `setBlocked`, `takeEaten`, `dropCurrent`, `control` | no equivalent (Round 4's rule changes) |

Steps:
1. Write a listener that receives events through `StageSession::subscribe()` (no team file is edited).
2. Write the adapter that fills the functions the scenes read from `StageSession`.
3. Write one `View` that draws a scene into a frame and sends it out as Sixel.
4. Add that view to `PlayScene`. `flow/*` is D's area, so ask for it with an issue.
5. Build the prototype's 10 fps clock by accumulating `update(dtMs)`.

Round 4's rule changes have to become new boss gimmicks on the team's side:

| Round 4 | Closest team gimmick |
|---|---|
| Stains block cells and eat pieces | Not `BlackoutGimmick` (which only hides). Cell blocking is new |
| Tentacles snatch the falling piece | Close to `ShapeShiftGimmick` (replaces the falling block) |
| The eye swaps left and right | New, because it changes input |

## 7. Building other stages

1. Subclass `Scene` for each round and add it to the list in `Game`.
2. Three functions are required: `drawBackground`, `drawNext` and `drawHud`.
3. The kid and mom come from `People.h`. For other ages, change the height and clothes.
4. An ending runs until `endingDone()` returns true. A fade to black is done in `drawEffects` by darkening the palette.
5. Keep one round = one photo piece and one memory item per round so it matches the team's photo progress.
6. The Stage 2 plan is four rounds: first day of school, middle school then a cutscene then high school, university, and the job interview as the boss.

## 8. Differences from the team's plan

These are places where this prototype and `docs/기획안.md` or the team's code do not match. Each one needs a team decision before the prototype is connected.

| Topic | Prototype | Team's plan or code | Change needed |
|---|---|---|---|
| Lines per photo piece | Rounds 1 to 3 are built for 10 lines, Round 1's gusts happen at lines 4 and 8 | `kLinesPerPiece` is 3 | Set it to 10 in `common/GameConfig.h` (D's file, needs an issue) |
| When the boss appears | In the last round (Round 4) | Photo 2/4 | Set `kBossAppearPieces` to 3 (D's file) |
| After each ending | Every round ends with a fade to black | The photo goes into the album | The album scene does not exist yet (D's area) |
| The story twist | Hidden until the end | Announced at the start | The screen side follows whichever the team picks |

## 9. Known limits

- The Visual Studio build on Windows was not verified from the environment this was written in. Only the four rounds playing to the end in `tools/sim.cpp` on Linux was checked.
- The connection to the team's `StageSession` is not written yet.
- There is no album scene and no map-walking scene (the start of the game and the end of each stage).
- Round 4's window does not open visibly.

## 10. Build and check

Visual Studio: make an empty C++ project, add every `.h` and `.cpp` in this folder (not `tools`), set C++17, and run Release x64. The detailed steps are in `README.txt`.

Check with the bot (Linux):

```
g++ -std=c++17 -O2 -I.. tools/sim.cpp Scene.cpp LakeRound.cpp KitchenRound.cpp BeachRound.cpp BedroomRound.cpp -o sim
./sim 4 3000
```

Keys (the same as the team's game where they overlap):

| Key | Action |
|---|---|
| Left / Right arrow, A / D | Move |
| Up arrow, W | Rotate |
| Down arrow, S | Move down one step |
| Space | Drop the block all the way down |
| Enter | Confirm: next round after a clear, retry after Game Over |
| Esc | Pause (press again to continue) |
| Q | Quit |

Testing keys that exist only in this prototype and do not collide with the team's keys: R restarts the round, F1 to F4 jump to round 1 to 4, E triggers the round's event (a gust of wind, a bite, a tentacle snatch). The team's pill keys (1 and 2) are not in the prototype.