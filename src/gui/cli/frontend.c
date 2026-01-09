/**
 * @file frontend.c
 * @brief Graphic interface implementation for Tetris game using ncurses
 * @details This file contains all visual rendering functions for the Tetris
 * game, including game board display, HUD elements, and various UI banners.
 */

#include <string.h>

#include "../../include/tetris.h"

/**
 * @brief Draws a rectangular frame using ncurses ACS characters
 * @param top_y Top Y coordinate of the rectangle
 * @param bottom_y Bottom Y coordinate of the rectangle
 * @param left_x Left X coordinate of the rectangle
 * @param right_x Right X coordinate of the rectangle
 * @details Draws a complete rectangle with corners and borders using
 * ACS (Alternative Character Set) symbols for consistent appearance.
 * The function creates a box with proper corners and edges.
 */
static void print_rectangle(int top_y, int bottom_y, int left_x, int right_x) {
  MVADDCH(top_y, left_x, ACS_ULCORNER); /**< Upper left corner */
  for (int i = left_x + 1; i < right_x; i++)
    MVADDCH(top_y, i, ACS_HLINE);        /**< Top horizontal line */
  MVADDCH(top_y, right_x, ACS_URCORNER); /**< Upper right corner */

  for (int i = top_y + 1; i < bottom_y; i++) {
    MVADDCH(i, left_x, ACS_VLINE);  /**< Left vertical line */
    MVADDCH(i, right_x, ACS_VLINE); /**< Right vertical line */
  }

  MVADDCH(bottom_y, left_x, ACS_LLCORNER); /**< Lower left corner */
  for (int i = left_x + 1; i < right_x; i++)
    MVADDCH(bottom_y, i, ACS_HLINE);        /**< Bottom horizontal line */
  MVADDCH(bottom_y, right_x, ACS_LRCORNER); /**< Lower right corner */
}

/**
 * @brief Prints the initial game overlay with borders and HUD elements
 * @details Creates the main game interface including game board border,
 * score panel, and introductory message. This is called once at game start.
 * The function sets up the complete static visual layout of the game.
 */
void print_overlay(void) {
  /** Draw main game board border */
  print_rectangle(0, BOARD_N + 1, 0, BOARD_M + 1);

  /** Draw status panel border on the right side */
  print_rectangle(0, BOARD_N + 1, BOARD_M + 2,
                  BOARD_M + STATUS_PANEL_WIDTH + 5);

  /** Draw individual boxes within status panel for different statistics */
  print_rectangle(1, 3, BOARD_M + 3,
                  BOARD_M + STATUS_PANEL_WIDTH + 4); /**< Current score box */
  print_rectangle(4, 7, BOARD_M + 3,
                  BOARD_M + STATUS_PANEL_WIDTH + 4); /**< High score box */
  print_rectangle(8, 10, BOARD_M + 3,
                  BOARD_M + STATUS_PANEL_WIDTH + 4); /**< Level box */

  /** Print static labels in the status panel */
  MVPRINTW(2, BOARD_M + 5, "SCORE");  /**< Score label */
  MVPRINTW(5, BOARD_M + 5, "HIGH");   /**< High score label (first line) */
  MVPRINTW(6, BOARD_M + 5, "SCORE");  /**< High score label (second line) */
  MVPRINTW(9, BOARD_M + 5, "LEVEL");  /**< Level label */
  MVPRINTW(13, BOARD_M + 4, "NEXT:"); /**< Next figure preview label */

  /** Print introductory message centered on the game board */
  MVPRINTW(BOARD_N / 2, (BOARD_M - INTRO_MESSAGE_LEN) / 2 + 1, INTRO_MESSAGE);
}

/**
 * @brief Prints current game statistics in the status panel
 * @details Updates the dynamic values in the status panel including current
 * score, high score, and level information from the game state. This function
 * should be called whenever game statistics change.
 */
void print_stats(void) {
  GameInfo_t *game = updateCurrentState();
  MVPRINTW(2, BOARD_M + 11, "%d",
           game->score); /**< Display current score value */
  MVPRINTW(6, BOARD_M + 11, "%d",
           game->high_score); /**< Display high score value */
  MVPRINTW(9, BOARD_M + 11, "%d",
           game->level); /**< Display current level value */
}

/**
 * @brief Renders the main game board with all placed blocks
 * @details Draws the entire game field showing all previously placed
 * tetrominoes. Combines with print_clear_figure() to show the complete game
 * state. This function updates the visual representation of the game field.
 */
void print_board(void) {
  GameInfo_t *game = updateCurrentState();

  /** Iterate through all cells of the game field (20 rows x 10 columns) */
  for (int i = 0; i < ROWS_MAP; i++)
    for (int j = 0; j < COLS_MAP; j++) {
      /** Determine block character based on cell content (1 = filled, 0 =
       * empty) */
      char *tray = (game->field[i][j] == 1) ? PIXEL_1 : PIXEL_0;
      /** Print block at calculated screen position with proper spacing */
      mvprintw(BOARDS_BEGIN + 1 + i, BOARDS_BEGIN + 1 + j * 3, tray);
    }

  /** Overlay the current active figure on top of the static board */
  print_clear_figure(PIXEL_1);
}

/**
 * @brief Renders or clears the current active tetromino on the game board
 * @param tray Character string to use for drawing the figure blocks (PIXEL_1 or
 * PIXEL_0)
 * @details Draws the currently active tetromino at its current position.
 * Can be used for both drawing (PIXEL_1) and clearing (PIXEL_0) the figure.
 * This function handles the visual representation of the moving tetromino.
 */
void print_clear_figure(char *tray) {
  int **figure = updateFigure();
  FigurePos_t *fig_pos = updateFigurePosition();

  /** Iterate through the figure matrix (4x4 for standard tetrominoes) */
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++)
      /** Only draw active blocks (value = 1) from the figure matrix */
      if (figure[i][j] == 1)
        /** Calculate screen position based on figure position and block size */
        mvprintw(BOARDS_BEGIN + 1 + fig_pos->y + i,
                 BOARDS_BEGIN + 1 + (fig_pos->x + j) * 3, tray);
}

/**
 * @brief Displays the next upcoming tetromino in the preview area
 * @details Renders the next figure in the designated preview area of the status
 * panel, allowing players to see what piece is coming next. This function
 * updates the "next figure" display.
 */
void clear_and_print_next_figure(void) {
  GameInfo_t *game = updateCurrentState();

  /** Iterate through the next figure matrix (4x4) */
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++) {
      /** Determine block character for next figure (1 = filled, 0 = empty) */
      char *patch = (game->next[i][j] == 1) ? PIXEL_1 : PIXEL_0;
      /** Print in the next figure preview area at fixed coordinates */
      mvprintw(17 + i, BOARD_M + 6 + j * 3, patch);
    }
}

/**
 * @brief Displays the game paused banner overlay
 * @details Shows a centered banner indicating the game is paused,
 * with instructions for resuming gameplay. This overlay appears
 * on top of the current game state.
 */
void print_pause_banner(void) {
  MVPRINTW(BOARD_N / 2 - 1, 1, "------------------------------");
  MVPRINTW(BOARD_N / 2, 1, "          GAME PAUSED         ");
  MVPRINTW(BOARD_N / 2 + 1, 1, "   press any key to continue  ");
  MVPRINTW(BOARD_N / 2 + 2, 1, "------------------------------");
}

/**
 * @brief Displays the game over banner overlay
 * @details Shows a centered banner when the game ends,
 * with instructions for exiting the game. This indicates
 * that the game has ended normally (board filled up).
 */
void print_gameover_banner(void) {
  MVPRINTW(BOARD_N / 2 - 1, 1, "------------------------------");
  MVPRINTW(BOARD_N / 2, 1, "           GAME OVER          ");
  MVPRINTW(BOARD_N / 2 + 1, 1, "     press any key to quit    ");
  MVPRINTW(BOARD_N / 2 + 2, 1, "------------------------------");
}

/**
 * @brief Displays an error banner overlay
 * @details Shows a centered banner when a critical error occurs,
 * indicating the game cannot continue. This is used for runtime
 * errors like file access issues or memory allocation failures.
 */
void print_exit_error_banner(void) {
  MVPRINTW(BOARD_N / 2 - 1, 1, "------------------------------");
  MVPRINTW(BOARD_N / 2, 1, "         ERROR OCCURED        ");
  MVPRINTW(BOARD_N / 2 + 1, 1, "     press any key to quit    ");
  MVPRINTW(BOARD_N / 2 + 2, 1, "------------------------------");
}
