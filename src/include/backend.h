/**
 * @file backend.h
 * @brief Backend module header for Tetris game logic
 * @details This header defines structures and functions for the core Tetris
 * game mechanics including game state management, figure manipulation, and
 * collision detection.
 */

#ifndef BACKEND_H
#define BACKEND_H

/**
 * @brief Structure representing figure position coordinates
 * @details Stores the current (x,y) position of the active tetromino on the
 * game field
 */
typedef struct {
  int x; /**< X-coordinate position on the game field */
  int y; /**< Y-coordinate position on the game field */
} FigurePos_t;

/**
 * @brief Structure containing complete game state information
 * @details Maintains all necessary data for the current game session including
 * field state, next figure preview, scoring, and game settings
 */
typedef struct {
  int **field;    /**< 2D array representing the game field/grid */
  int **next;     /**< 2D array representing the next upcoming figure */
  int score;      /**< Current player score */
  int high_score; /**< All-time high score */
  int level;      /**< Current game level */
  int speed;      /**< Current game speed (falling rate) */
  int pause;      /**< Pause state flag (0 = running, 1 = paused) */
} GameInfo_t;

// ====================
// State Management Functions
// ====================

/**
 * @brief Retrieves the current game state information
 * @return Pointer to the current GameInfo_t structure
 * @details Provides access to the global game state containing field data,
 * scores, and game settings. Returns a singleton instance.
 */
GameInfo_t *updateCurrentState(void);

/**
 * @brief Retrieves the current active figure data
 * @return Pointer to a 2D array representing the current tetromino
 * @details Returns the figure matrix that is currently active and being
 * manipulated
 */
int **updateFigure(void);

/**
 * @brief Retrieves the current figure position
 * @return Pointer to the current FigurePos_t structure
 * @details Provides access to the (x,y) coordinates of the active tetromino
 */
FigurePos_t *updateFigurePosition(void);

/**
 * @brief Main game loop function
 * @details Controls the primary game execution flow, handling state
 * transitions, user input processing, and rendering updates. This is the core
 * function that runs the entire game session.
 */
void game_loop(void);

// ====================
// Game Initialization & Cleanup
// ====================

/**
 * @brief Initializes the game state and resources
 * @return int Error code (0 = success, non-zero = error)
 * @details Initializes random number generation, ncurses window, locale
 * settings
 */
int init_game(void);

/**
 * @brief Frees resourses and exit ncurses
 * @details Frees the game field, initial figures, scores, and other game
 * parameters. Frees ncurses
 */
void exit_game();

/**
 * @brief Frees memory allocated for the game field
 * @param field Pointer to the 2D field array to free
 * @details Properly deallocates memory used by the game field matrix
 */
void free_field(int **field);

/**
 * @brief Completely frees all game-related resources
 * @details Cleans up all dynamically allocated memory including field, figures,
 * and state data
 */
void free_game(void);

// ====================
// Figure Management
// ====================

/**
 * @brief Generates and assigns the next tetromino figure
 * @details Randomly selects and prepares the next figure to be spawned
 */
void assign_next_figure(void);

/**
 * @brief Copies the next figure to become the current active figure
 * @details Transfers the prepared next figure to the active figure slot
 */
void copy_next_figure_to_figure(void);

/**
 * @brief Initializes the starting position for a new figure
 * @details Places the active figure at the top-center of the game field
 */
void init_figure_position(void);

/**
 * @brief Rotates the specified figure 90 degrees clockwise
 * @param figure Pointer to the 2D figure array to rotate
 * @details Performs matrix rotation operation on the tetromino
 */
void rotate_figure(int **figure);

// ====================
// Game Logic Functions
// ====================

/**
 * @brief Checks for collisions between figure and field boundaries/blocks
 * @return int Collision status (0 = no collision, 1 = collision detected)
 * @details Verifies if the current figure position is valid and not overlapping
 */
int check_collide(void);

/**
 * @brief Attaches the current figure to the game field
 * @details Permanently places the active figure onto the game grid
 */
void attach_figure_to_field(void);

/**
 * @brief Checks for and removes completed rows
 * @return int Number of rows destroyed
 * @details Scans the game field for filled rows, removes them, and shifts down
 * above rows
 */
int destruction_of_rows(void);

/**
 * @brief Updates high score from file if current score exceeds it
 * @return int Error code (0 = success, non-zero = error)
 * @details Reads high score from file, updates if necessary, and saves back
 */
int high_score_update(void);

/**
 * @brief Recalculates game statistics after row destruction
 * @param n_rows Number of rows destroyed in the last operation
 * @details Updates score, level, and speed based on rows cleared
 */
void recalculate_stats(int n_rows);

#endif /* BACKEND_H */
