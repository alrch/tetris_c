/**
 * @file tetris.h
 * @brief Main Tetris game header - aggregates all module interfaces
 * @details This is the primary include file for the Tetris game that brings
 * together all component modules: backend logic, finite state machine, frontend
 * display, and configuration definitions. It serves as the main entry point for
 * the game API.
 */

#ifndef TETRIS_H
#define TETRIS_H

/**
 * @defgroup core_modules Core Game Modules
 * @brief Essential components that make up the Tetris game
 */

/**
 * @ingroup core_modules
 * @brief Backend game logic and state management
 */
#include "backend.h"

/**
 * @ingroup core_modules
 * @brief Game configuration constants and macros
 */
#include "defines.h"

/**
 * @ingroup core_modules
 * @brief Finite State Machine for game flow control
 */
#include "fsm.h"

/**
 * @brief Conditional compilation for testing vs production
 * @details When USE_MOCK is defined, uses mock ncurses for unit testing.
 * Otherwise, includes the real ncurses library and frontend modules.
 */
#ifndef USE_MOCK
#include <ncurses.h> /**< Real ncurses library for terminal UI */

/**
 * @ingroup core_modules
 * @brief Frontend display and rendering functions
 */
#include "frontend.h"
#else
#include "mock_ncurses.h" /**< Mock ncurses for unit testing */
#endif

#endif /* TETRIS_H */

/**
 * @mainpage Tetris Architecture Overview
 *
 * @dot
 * digraph architecture {
 *   rankdir=TB;
 *   node [shape=rectangle];
 *
 *   "tetris.h" -> "backend.h";
 *   "tetris.h" -> "fsm.h";
 *   "tetris.h" -> "frontend.h";
 *   "tetris.h" -> "defines.h";
 *   "backend.h" -> "defines.h";
 *   "frontend.h" -> "defines.h";
 *   "fsm.h" -> "defines.h";
 * }
 * @enddot
 */
