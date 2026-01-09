/**
 * @file defines.h
 * @brief Global constants and macro definitions for Tetris game
 * @details This header contains all configuration constants, macro definitions,
 * and system settings used throughout the Tetris game implementation.
 */

#ifndef DEFINES_H
#define DEFINES_H

// ====================
// NCurses Configuration Macros
// ====================

/**
 * @brief Initializes ncurses library with common settings
 * @param time Timeout value for input blocking (milliseconds)
 * @details Macro that encapsulates standard ncurses initialization:
 * - Initialize screen
 * - Disable character echoing
 * - Hide cursor
 * - Enable special key processing
 * - Set input timeout
 */
#define NCURSES_INIT(time) \
  {                        \
    initscr();             \
    noecho();              \
    curs_set(0);           \
    keypad(stdscr, TRUE);  \
    timeout(time);         \
  }

// ====================
// Game Configuration Constants
// ====================

/**
 * @brief Maximum achievable game level
 */
#define MAX_LEVEL 10

/**
 * @brief Initial timeout for figure falling speed (milliseconds)
 */
#define INITIAL_TIMEOUT 500

/**
 * @brief Time decrement per level increase (milliseconds)
 * @details How much faster the game gets with each level
 */
#define SPEED_DECREMENT 30

/**
 * @brief File path for storing high score records
 */
#define HIGH_SCORE_FILE "./out/record_note.txt"

// ====================
// Keyboard Input Codes
// ====================

/** @brief Down arrow key code */
#define KEY_DOWN 0402

/** @brief Up arrow key code */
#define KEY_UP 0403

/** @brief Left arrow key code */
#define KEY_LEFT 0404

/** @brief Right arrow key code */
#define KEY_RIGHT 0405

/** @brief Escape key code */
#define ESCAPE 27

/** @brief Enter key code */
#define ENTER_KEY 10

// ====================
// Game Geometry Constants
// ====================

/**
 * @brief Number of available tetromino figures
 */
#define NUMBER_OF_FIGURES 7

/**
 * @brief Size of the figure matrix (4x4 for standard tetrominoes)
 */
#define SIDE_OF_FIGURE_SQUARE 4

/**
 * @brief Number of rows in the game field
 */
#define ROWS_MAP 20

/**
 * @brief Number of columns in the game field
 */
#define COLS_MAP 10

// ====================
// Display and Rendering Constants
// ====================

/**
 * @brief Starting offset for game board rendering
 */
#define BOARDS_BEGIN 2

/**
 * @brief Initial X position for spawning figures (centered)
 */
#define FIGURESTART_X ((COLS_MAP - SIDE_OF_FIGURE_SQUARE) / 2)

/**
 * @brief Initial Y position for spawning figures (top)
 */
#define FIGURESTART_Y 0

/**
 * @brief String representation of a filled block
 */
#define PIXEL_1 "[X]"

/**
 * @brief String representation of an empty block
 */
#define PIXEL_0 "   "

/**
 * @brief Introductory message displayed at game start
 */
#define INTRO_MESSAGE "Press R to start!"

/**
 * @brief Length of the introductory message string
 */
#define INTRO_MESSAGE_LEN 17

// ====================
// Calculated Display Constants
// ====================

/**
 * @brief Total board height in character rows
 * @details Based on ROWS_MAP
 */
#define BOARD_N (ROWS_MAP)

/**
 * @brief Total board width in character columns
 * @details COLS_MAP * 3 because each block is 3 characters wide
 */
#define BOARD_M (COLS_MAP * 3)

/**
 * @brief Width of the status panel area
 */
#define STATUS_PANEL_WIDTH 13

// ====================
// Positioning Macros
// ====================

/**
 * @brief Macro for printing text with board offset
 * @param y Y coordinate relative to board start
 * @param x X coordinate relative to board start
 * @param ... Format string and arguments for mvprintw
 */
#define MVPRINTW(y, x, ...) \
  mvprintw(BOARDS_BEGIN + (y), BOARDS_BEGIN + (x), __VA_ARGS__)

/**
 * @brief Macro for adding characters with board offset
 * @param y Y coordinate relative to board start
 * @param x X coordinate relative to board start
 * @param c Character to display
 */
#define MVADDCH(y, x, c) mvaddch(BOARDS_BEGIN + (y), BOARDS_BEGIN + (x), c)

// ====================
// Error Code Constants
// ====================

/** @brief Success code (no error) */
#define NO_ERROR 0

/** @brief General error code */
#define ERROR 1

#endif /* DEFINES_H */
