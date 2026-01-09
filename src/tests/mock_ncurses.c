/**
 * @file mock_ncurses.c
 * @brief Mock implementation of ncurses functions for unit testing
 * @details This file provides stub implementations of ncurses functions and
 * frontend rendering functions to allow unit testing without requiring
 * a terminal display. All functions are no-ops or return mock data.
 */

#include <stdio.h>

#include "../include/tetris.h"

// ====================
// Frontend Function Stubs
// ====================

/**
 * @brief Mock implementation of print_overlay - no operation
 * @details Empty stub for initial game overlay rendering
 */
void print_overlay(void) {};

/**
 * @brief Mock implementation of clear_and_print_next_figure - no operation
 * @details Empty stub for next figure preview rendering
 */
void clear_and_print_next_figure(void) {};

/**
 * @brief Mock implementation of print_board - no operation
 * @details Empty stub for game board rendering
 */
void print_board(void) {};

/**
 * @brief Mock implementation of print_pause_banner - no operation
 * @details Empty stub for pause banner display
 */
void print_pause_banner(void) {};

/**
 * @brief Mock implementation of print_gameover_banner - no operation
 * @details Empty stub for game over banner display
 */
void print_gameover_banner(void) {};

/**
 * @brief Mock implementation of print_exit_error_banner - no operation
 * @details Empty stub for error banner display
 */
void print_exit_error_banner(void) {};

/**
 * @brief Mock implementation of print_stats - no operation
 * @details Empty stub for statistics display
 */
void print_stats(void) {};

/**
 * @brief Mock implementation of print_clear_figure - no operation
 * @param tray Character tray parameter (unused)
 * @details Empty stub for figure rendering/clearing
 */
void print_clear_figure(char *tray) { (void)tray; };

// ====================
// Mock NCurses Variables
// ====================

/**
 * @brief Mock standard screen window structure
 * @details Provides a dummy WINDOW structure for testing
 */
WINDOW mock_win = {10, 20}; /**< Mock window with basic dimensions */

/**
 * @brief Mock standard screen pointer
 * @details Points to the mock window structure for ncurses compatibility
 */
WINDOW *stdscr = &mock_win; /**< Global standard screen pointer */

// ====================
// Mock NCurses Functions
// ====================

/**
 * @brief Mock implementation of wgetch
 * @param win Window parameter (unused)
 * @return Always returns 'q' character for consistent test behavior
 * @details Provides predictable input for testing without real keyboard input
 */
int wgetch(WINDOW *win) {
  (void)win;  /**< Suppress unused parameter warning */
  return 'q'; /**< Return quit character for test termination */
}

/**
 * @brief Mock implementation of wtimeout
 * @param win Window parameter (unused)
 * @param delay Timeout delay in milliseconds (logged but not used)
 * @details Logs the timeout value for verification in tests without
 * actually implementing timeout functionality
 */
void wtimeout(WINDOW *win, int delay) {
  (void)win;   /**< Suppress unused parameter warning */
  (void)delay; /**< Suppress unused parameter warning */
  printf("Timeout set to %d milliseconds\n", delay); /**< Log timeout value */
}

// ====================
// Additional Mock Functions (if needed)
// ====================

/**
 * @brief Mock implementation of initscr
 * @return Pointer to mock standard screen
 * @details Initializes mock ncurses environment for testing
 */
WINDOW *initscr(void) {
  printf("Mock ncurses initialized\n");
  return stdscr;
}

/**
 * @brief Mock implementation of endwin
 * @return Always returns OK (0)
 * @details Cleans up mock ncurses environment
 */
int endwin(void) {
  printf("Mock ncurses terminated\n");
  return 0; /**< Return OK status */
}

/**
 * @brief Mock implementation of noecho
 * @details Disables echoing in mock environment
 */
void noecho(void) { printf("Echo disabled\n"); }

/**
 * @brief Mock implementation of curs_set
 * @param visibility Cursor visibility setting
 * @return Previous cursor setting (always 0 in mock)
 * @details Controls cursor visibility in mock environment
 */
int curs_set(int visibility) {
  printf("Cursor visibility set to %d\n", visibility);
  return 0; /**< Return previous setting */
}

/**
 * @brief Mock implementation of keypad
 * @param win Window parameter (unused)
 * @param enable Keypad enable flag
 * @return Always returns OK (0)
 * @details Enables/disables keypad functionality
 */
int keypad(WINDOW *win, int enable) {
  (void)win;
  printf("Keypad %s\n", enable ? "enabled" : "disabled");
  return 0; /**< Return OK status */
}
