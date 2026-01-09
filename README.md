# Tetris CLI (C)

A terminal-based implementation of the classic Tetris game using a Finite State
Machine and layered architecture.

## Project Goals

* Implement Tetris in the terminal with a clear, maintainable architecture  
* Apply **Finite State Machine (FSM)** to manage game states  
* Gain proficiency in **C** development with emphasis on modularity and
testability  
* Decouple game logic from input/output and presentation layers  
* Achieve >90% unit test coverage  
* Gain hands-on experience with **ncurses** for terminal control  
* Learn documentation generation using **Doxygen and LaTeX**  
* Practice writing code compliant with auto-documentation requirements,
including an intentionally excessive number of comments for a small project
(partially AI-assisted to accelerate development)

## Tech Stack

- **C11** — primary programming language  
- **ncurses** — terminal control and UI rendering  
- **Check** — unit testing framework  
- **Makefile** — project build, execution, and testing  
- **Finite State Machine (FSM)** — game lifecycle orchestration  
- **Doxygen + LaTeX** — technical documentation generation from source code  

## Quick Start

```bash
git clone https://gitlab.com/alrch/tetris-c
cd tetris-c/src

# Install dependencies (Ubuntu/Debian)
sudo apt install libncurses5-dev libcheck-dev

# Build and run
make run

# Build only
make build

# Run directly from binaries
./build/tetris
```

Controls:
- **R** — start game  
- **← →** — move piece horizontally  
- **↑** — rotate piece  
- **↓** — soft drop (accelerated fall)  
- **P** — pause / resume  
- **Q** — quit game  

## Features

* Full Tetris mechanics: falling pieces, rotation, line clearing, scoring  
* **Pause** support with seamless resume  
* Automatic speed increase as level progresses  
* Clear separation of **logic**, **state**, and **presentation**  
* Safe timer and signal handling (`SIGALRM`)  
* Graceful shutdown on game end or error  

## Architecture

The project follows **layered architecture** principles and uses a
**Finite State Machine** to orchestrate state transitions.

### High-Level Diagram

```md
┌───────────────────┐
│    Presentation   │  ncurses, rendering, input
│   (UI Layer)      │
└─────────┬─────────┘
          │
          ▼
┌───────────────────┐
│     Game Loop     │  FSM: START → SPAWN → MOVE → ...
│   & State Machine │
└─────────┬─────────┘
          │
          ▼
┌───────────────────┐
│    Domain Logic   │  Board, Tetromino, collision,
│   (Core Rules)    │  line clearing, scoring
└─────────┬─────────┘
          │
          ▼
┌───────────────────┐
│  Infrastructure   │  Timer, signal handling,
│                   │  memory management
└───────────────────┘
```

### Project Structure

```md
src/
├── brick_game/
│   └── tetris/              # Core game logic and state management
│       ├── backend.c        # Game rules, board state, collision detection,
│                            # line clearing, scoring, figure handling
│       ├── fsm.c            # Finite State Machine implementation
│       └── tetris.c         # Main entry point (`main()`) and game loop
├── gui/
│   └── cli/
│       └── frontend.c       # Terminal UI rendering using ncurses
├── include/                 # Public header files
│   ├── backend.h            # Core game logic interface
│   ├── defines.h            # Constants, macros, and configuration
│   ├── frontend.h           # UI rendering function declarations
│   ├── fsm.h                # FSM states and input action definitions
│   ├── mock_ncurses.h       # Stubs for ncurses functions (used in tests)
│   └── tetris.h             # Master header — includes all modules
├── tests/
│   ├── mock_ncurses.c       # Mock implementations of ncurses for testing
│   └── tests.c              # Unit tests using the Check framework
└── Makefile                 # Build, test, documentation, and analysis targets
```

## Layers and Responsibilities

### Core (Domain)

* Models: `Board`, `Tetromino`, `GameState`  
* Logic includes:
  - collision detection  
  - full-line removal  
  - score and level calculation  
  - new piece generation  
* Fully **decoupled from ncurses or input devices**

### FSM (Game State Machine)

* States: `START`, `SPAWN`, `MOVE`, `LOCK`, `LINE_CLEAR`, `PAUSE`, `GAME_OVER`  
* Explicit, deterministic state transition rules  
* Side effects isolated (e.g., timer tick → transition to `MOVE`)  
* Simplifies testing and future extensions (e.g., adding a main menu)

### UI Layer

* Initializes and cleans up `ncurses`  
* Renders game board, next piece preview, score, and level  
* Captures and interprets user input (keyboard events)  
* Dispatches actions to the FSM (`KEY_LEFT`, `KEY_PAUSE`, etc.)

### Utils

* POSIX timer setup (`timer_create`, `SIGALRM`)  
* Safe memory allocation and deallocation  
* Signal handling for clean exit on interruption

### Testing

* Unit tests cover only **pure logic** (core, FSM)  
* **Check** framework used for isolated, repeatable test cases  
* No direct UI testing (ncurses functions are mocked)  
* Critical scenarios covered:
  - correct line clearing  
  - piece locking on collision  
  - valid rotation logic  
  - FSM transitions during pause/resume  

Run tests:
```bash
make test
```

> **Note**: Tests were written manually—no AI or auto-generation was used—to
ensure precise scenario control and validation quality.