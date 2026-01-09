/**
 * @file fsm.h
 * @brief Finite State Machine (FSM) interface for Tetris game logic
 * @details This header defines the state machine that controls the game flow,
 * including all possible game states, user actions, and state transition
 * functions.
 */

#ifndef FSM_H
#define FSM_H

#include <stdbool.h>

/**
 * @brief Enumeration of all possible game states in the Tetris state machine
 * @details Defines the complete lifecycle of a Tetris game session
 */
typedef enum {
  START = 0, /**< Initial state, waiting for game start */
  SPAWN,     /**< Spawning a new tetromino */
  MOVING,    /**< Active state where tetromino is moving down */
  SHIFTING,  /**< Processing lateral movement or rotation */
  ATTACHING, /**< Finalizing tetromino placement on the field */
  GAMEOVER,  /**< Game has ended normally */
  EXIT_ERROR /**< Game terminated due to an error condition */
} TetrisState_t;

/**
 * @brief Enumeration of all possible user input actions
 * @details Maps physical user inputs to logical game actions
 */
typedef enum {
  Start = 0, /**< Start the game (R key) */
  Pause,     /**< Pause/unpause the game (P key) */
  Terminate, /**< Terminate the game (ESC key) */
  Left,      /**< Move tetromino left (left arrow) */
  Right,     /**< Move tetromino right (right arrow) */
  Up,        /**< Immediate drop (up arrow) */
  Down,      /**< Accelerate fall (down arrow) */
  Action,    /**< Rotate tetromino (space bar) */
  No_signal  /**< No user input received */
} UserAction_t;

/**
 * @brief Retrieves the current game state pointer
 * @return Pointer to the current TetrisState_t value
 * @details Provides access to the global game state variable.
 * The returned pointer can be used to read or modify the current state.
 */
TetrisState_t *updateTetrisState(void);

/**
 * @brief Processes user input based on current game state
 * @param action The user action to process
 * @param hold Indicates if the action is being held (for future repeat input
 * handling)
 * @details This is the main state transition function that routes user actions
 * to the appropriate state-specific handler. The state machine advances based
 * on the current state and received input.
 */
void userInput(UserAction_t action, bool hold);

/**
 * @brief Maps raw input codes to logical game actions
 * @param user_input The raw input code from ncurses
 * @return Corresponding UserAction_t value
 * @details Translates keyboard codes (KEY_UP, KEY_DOWN, etc.) into
 * the game's internal action representation. Returns No_signal for unmapped
 * inputs.
 */
UserAction_t get_action(int user_input);

#endif /* FSM_H */
