
/**
 * @file tetris.c
 * @brief Main game file containing calls to initialization, main game loop and
 * exit game
 * @details This file contains the main function and core game loop for the
 * Tetris game. It handles initialization, game state management, and cleanup
 * functions calls.
 */

#include "../../include/tetris.h"

#include <stdlib.h>
#include <time.h>

/**
 * @brief Main entry point of the Tetris game
 * @return int Returns NO_ERROR (0) on successful execution
 *
 * @details Starts the main game loop
 */
int main(void) {
  int error = init_game();
  if (error == NO_ERROR) {
    game_loop();
    exit_game();
  }
  return error;
}
