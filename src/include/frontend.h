/**
 * @file frontend.h
 * @brief Frontend interface for Tetris game graphics rendering
 * @details This header declares all functions responsible for visual output
 * and user interface rendering using the ncurses library.
 */

#ifndef FRONTEND_H
#define FRONTEND_H

/**
 * @brief Prints the initial game overlay with borders and static UI elements
 * @details Creates the main game interface including game board border,
 * status panel, and introductory message. Called once at game startup.
 */
void print_overlay(void);

/**
 * @brief Displays the next upcoming tetromino in the preview area
 * @details Renders the next figure in the designated preview area,
 * allowing players to see what piece is coming next.
 */
void clear_and_print_next_figure(void);

/**
 * @brief Renders the main game board with all placed blocks
 * @details Draws the entire game field showing all previously placed
 * tetrominoes. Should be called whenever the game field needs visual updating.
 */
void print_board(void);

/**
 * @brief Displays the game paused banner overlay
 * @details Shows a centered banner indicating the game is paused,
 * with instructions for resuming gameplay.
 */
void print_pause_banner(void);

/**
 * @brief Displays the game over banner overlay
 * @details Shows a centered banner when the game ends normally,
 * with instructions for exiting the game.
 */
void print_gameover_banner(void);

/**
 * @brief Displays an error banner overlay
 * @details Shows a centered banner when a critical error occurs,
 * indicating the game cannot continue.
 */
void print_exit_error_banner(void);

/**
 * @brief Updates and displays current game statistics
 * @details Refreshes the dynamic values in the status panel including
 * current score, high score, and level information.
 */
void print_stats(void);

/**
 * @brief Renders or clears the current active tetromino on the game board
 * @param tray Character string to use for drawing the figure blocks
 *        (PIXEL_1 for drawing, PIXEL_0 for clearing)
 * @details Handles the visual representation of the moving tetromino.
 * Can be used for both drawing and clearing the figure from the screen.
 */
void print_clear_figure(char *tray);

#endif /* FRONTEND_H */
