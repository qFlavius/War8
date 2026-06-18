# War8 — Război în 8

**War8** is a turn-based abstract strategy board game built in **C++17** with **SFML 3.0.2**.  
The project implements the Romanian rational game **“Război în 8”** (*War in Eight*) from Nicolae Oprișiu’s *Olimpiada Jocurilor Raționale* (*The Olympiad of Rational Games*, Chapter 10).

The game was developed as an academic project for the **Introduction to Programming** course during the first year of Computer Science.

---

## Overview

In *Război în 8*, two players control armies of eight pieces on an 8×8 board. Pieces move one step diagonally to an empty square. The main tactical idea is to **block opponent pieces**: after the capture rule becomes active, opponent pieces with no legal moves are removed from the board.

This digital version keeps the original strategic idea and adapts it into an interactive SFML application with multiple play modes, AI opponents, timers, themes, audio, settings, and a leaderboard.

---

## Features

### Gameplay
- **Player vs Player**
- **Player vs Computer**
- **Computer vs Computer**
- Standard 8×8 board with 8 pieces per side
- Legal movement: one diagonal step to an empty square
- Automatic capture-by-blocking rule
- Optional **per-turn timer** in `MM:SS` format
- Timeout behavior: the player loses the turn instead of losing the whole match
- Game-over handling with winner/draw detection

### Artificial Intelligence
- Separate AI module in `AI/`
- **Easy AI**
  - One-ply heuristic evaluation
  - Considers material, mobility, center positioning, and self-blocking
  - Uses pseudo-random tie-breaking for more varied behavior
- **Hard AI**
  - Time-bounded minimax search
  - Alpha-beta pruning
  - Iterative deepening for stable decisions under time limits
  - Evaluation based on material advantage, mobility, blocked pieces, and board positioning

### User Interface
- State-based Play flow:
  1. Choose game mode
  2. Enter player names
  3. Configure timer and AI difficulty
  4. Play the match
  5. View the result screen
- Detailed in-game HUD:
  - Current turn
  - Remaining turn time
  - Total elapsed match time
  - White/Black piece counts
  - White/Black capture counts
- Mouse-based piece selection and highlighted legal moves
- Game-over overlay with visual effects

### Additional Systems
- Theme system with multiple visual themes
- Custom theme persistence
- Settings menu for FPS, sound, music, and language
- English/Romanian translation support
- File-based leaderboard
- Learn section with rule explanations and diagrams
- Sound effects and background music

---

## Game Rules Implemented

This implementation follows the core rules of *Război în 8*:

- The game is played by two players on an **8×8 board**.
- Each player starts with **8 pieces**.
- A piece may move **one square diagonally** in any direction, but only to an empty square.
- After a configurable opening threshold (`captureAfterPly`), blocked opponent pieces are automatically captured.
- A piece is blocked when it has **no legal diagonal move**.
- The winner is determined by the number of pieces remaining when the game ends.

> Note: In the original book, blocked pieces may be removed when observed by players. In this digital version, captures are applied automatically to make the gameplay deterministic and easier to follow.

---

## Project Structure

```text
War8-main/
├── AI/
│   ├── AIPlayer.cpp        # Easy/Hard AI implementation
│   └── AIPlayer.hpp        # AI public interface
├── LeaderBoard/
│   ├── leaderboard.hpp     # Leaderboard screen and file loading
│   └── leaderboard.txt     # Stored leaderboard data
├── Learn/
│   ├── learn.hpp           # Learn/rules screen
│   └── fig*.png            # Rule illustrations
├── MainMenu/
│   └── MainMenu.hpp        # Main menu screen
├── PlayScreen/
│   ├── PlayScreen.cpp      # Main gameplay logic and HUD
│   └── PlayScreen.hpp
├── Profile/
│   ├── playerData.hpp      # Settings/theme persistence
│   └── playerData.txt
├── Settings/
│   └── settings.hpp        # Settings menu
├── SoundEffects/
│   ├── SoundEffects.hpp
│   ├── click.wav
│   └── KeyBoardKey.wav
├── Themes/
│   ├── Themes.hpp          # Theme selection UI
│   ├── themesRead.cpp/h    # Theme file parsing
│   └── theme assets
├── lang/
│   ├── Translations.cpp
│   └── Translations.hpp
├── GameConfig.hpp          # Game mode, player info, difficulty config
├── globalVar.hpp           # Shared global settings/helpers
├── main.cpp                # Application entry point and screen routing
├── war8.sln
└── war8.vcxproj
```

---

## Technical Highlights

### State-driven game flow
The `PlayScreen` module works as a small state machine, using phases for mode selection, name entry, option configuration, gameplay, and game-over display.

### Board representation
The board is stored as a compact 8×8 matrix:

```cpp
std::uint8_t board[8][8];
```

where:
- `0` = empty square
- `1` = white piece
- `2` = black piece

### Mouse-to-board mapping
Mouse clicks are converted from screen coordinates into board cells, allowing the player to select pieces and move them to highlighted legal squares.

### AI separation
The AI is isolated from the UI and game rendering. `PlayScreen` asks the AI for a move, and the AI returns a simple `AIMove` structure. This keeps the decision-making logic independent from the SFML drawing code.

### File-based persistence
The project uses text files to persist:
- leaderboard scores
- theme selection
- FPS/music/sound/language settings
- custom theme colors

---

## Requirements

- **Windows**
- **Visual Studio 2022** or newer
- **C++17**
- **SFML 3.0.2**

The Visual Studio project is configured for SFML static linking using this path:

```text
lib/SFML-3.0.2/
```

If SFML is installed elsewhere, update the Visual Studio project properties:
- C/C++ → Additional Include Directories
- Linker → Additional Library Directories
- Linker → Input → Additional Dependencies

---

## Build and Run

1. Clone the repository:

```bash
git clone <repository-url>
cd War8-main
```

2. Install or extract **SFML 3.0.2** into:

```text
lib/SFML-3.0.2/
```

3. Open the solution:

```text
war8.sln
```

4. Select:

```text
x64 | Debug
```

or:

```text
x64 | Release
```

5. Build and run the project from Visual Studio.

Make sure the working directory is the project root so that fonts, sounds, themes, and text files are loaded correctly.

---

## Controls

### Menu / Setup
- Mouse click: select options
- `1`, `2`, `3`: choose game mode
- `Enter`: continue / start
- `Tab`: switch name input field
- `Backspace`: delete character
- `B`: go back
- `Esc`: return to main menu

### Gameplay
- Click a piece to select it
- Click a highlighted square to move
- `R`: restart/rematch
- `Esc`: return to main menu

---

## What I Learned

During this project, I practiced and improved:

- event-driven programming with SFML
- real-time rendering and UI layout
- mouse input handling and coordinate mapping
- state-machine based screen flow
- implementing board game rules and edge cases
- AI decision-making with heuristics, minimax, alpha-beta pruning, and time budgets
- file I/O for persistence
- modular C++ project organization
- debugging and iterative feature development with Git

---

## Academic Context

This project was developed for the **Introduction to Programming** course, studied in the first semester of the first year at the Faculty of Computer Science, “Alexandru Ioan Cuza” University of Iași.

---

## Attribution

The game rules are based on **“Război în 8”** from:

**Nicolae Oprișiu — *Olimpiada Jocurilor Raționale* (*The Olympiad of Rational Games*), Chapter 10.**

“War in Eight” is used here as an English rendering of the Romanian title. The original Romanian title is kept for accuracy and searchability.

---

## Status

The project is complete as an academic implementation and includes the main gameplay loop, AI opponents, UI screens, themes, settings, translations, sound, and persistence.
