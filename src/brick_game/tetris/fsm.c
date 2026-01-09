/**
 * @file fsm.c
 * @brief Finate state machine logic
 * @details This file implements the state machine that controls the game flow
 * of Tetris, function with static object of game state. It handles transitions
 * between different game states and processes user input accordingly.
 */

#include "../../include/tetris.h"

static void on_start_state(UserAction_t signal);
static void on_spawn_state(void);
static void on_moving_state(UserAction_t signal);
static void on_shifting_state(void);
static void on_attaching_state(void);
static void on_gameover_state(void);
static void on_exit_error_state(void);

static void __attribute__((unused)) moveup(void);
static void movedown(void);
static void moveright(void);
static void moveleft(void);
static void rotate_action(void);
static void pause_game(void);

/**
 * @brief Keeps static object: current game state
 *
 * @return Pointer to curent game state
 */
TetrisState_t *updateTetrisState(void) {
  static TetrisState_t state = 0;
  return &state;
}

/**
 * @brief Processes user input based on current game state
 * @param[in] action The user action to process
 * @param[in] hold Indicates if the action is being held (repeat)
 * @details This is the main input handler that routes user actions
 * to the appropriate state-specific handler function. The hold parameter
 * is currently unused but reserved for future input handling improvements.
 *
 * @note The hold parameter is cast to void to suppress unused parameter
 * warnings
 */
void userInput(UserAction_t action, bool hold) {
  TetrisState_t *state = updateTetrisState();
  (void)hold;
  switch (*state) {
    case START:
      on_start_state(action);
      break;
    case SPAWN:
      on_spawn_state();
      break;
    case MOVING:
      on_moving_state(action);
      break;
    case SHIFTING:
      on_shifting_state();
      break;
    case ATTACHING:
      on_attaching_state();
      break;
    case GAMEOVER:
      on_gameover_state();
      break;
    case EXIT_ERROR:
      on_exit_error_state();
      break;
      // default:
      //   break;
  }
}

/**
 * @brief On START state: awaiting input from user to change to other state
 * @param[in] signal The user input provcessed to signal
 * @details Awaits user input and updates state after it either to GAMEOVER or
 * to SPAWN
 */
static void on_start_state(UserAction_t signal) {
  TetrisState_t *state = updateTetrisState();
  switch (signal) {
    case Start:
      assign_next_figure();
      *state = SPAWN;
      break;
    case Terminate:
      *state = GAMEOVER;
      break;
    default:
      *state = START;
      break;
  }
}

/**
 * @brief On SPAWN state: spawns new figure
 * @details Copies next figure to current figure, updating both, prints updated
 * board and figure. If collision of spawned figure then switches state to
 * GMAEOVER, else to MOVING state. Sets up timeout for current round, depending
 * on game info level
 *
 * @note USE_MOCK used for frontend stubs for unit tests of logic
 */
static void on_spawn_state(void) {
  TetrisState_t *state = updateTetrisState();
#ifndef USE_MOCK
  GameInfo_t *game = updateCurrentState();
  timeout(game->speed);
#endif
  if (high_score_update() == NO_ERROR) {
    copy_next_figure_to_figure();
    assign_next_figure();
    clear_and_print_next_figure();
    init_figure_position();
    print_board();
    print_stats();
    *state = (check_collide()) ? GAMEOVER : MOVING;
  } else
    *state = EXIT_ERROR;
}

/**
 * @brief On MOVING state: awaiting user input for set timeout() period of time
 * @details Awaits user input and calls appropriate function based on it: try to
 * change position or rotate, pause or terminate game. Then game state goes to
 * SHIFTING/PAUSE/GAMEOVER
 */
static void on_moving_state(UserAction_t signal) {
  TetrisState_t *state = updateTetrisState();
  switch (signal) {
    case Up:
      moveup();
      break;
    case Down:
      movedown();
      break;
    case Right:
      moveright();
      break;
    case Left:
      moveleft();
      break;
    case Action:
      rotate_action();
      break;
    case Pause:
      pause_game();
      break;
    case Terminate:
      *state = GAMEOVER;
      break;
    default:
      break;
  }

  if (*state != GAMEOVER && *state != EXIT_ERROR) {
    *state = SHIFTING;
  }
}

/**
 * @brief On SHIFTING state: shifts figure down if it is possible
 * @details If moving figure down causes collision then changes game state to
 * ATTACHING, otherwise change figure position and game state to MOVING. Prints
 * updated board and figure
 */
static void on_shifting_state(void) {
  TetrisState_t *state = updateTetrisState();
  FigurePos_t *fig_pos = updateFigurePosition();
  fig_pos->y++;
  if (check_collide()) {
    fig_pos->y--;
    *state = ATTACHING;
  } else {
    *state = MOVING;
    fig_pos->y--;
    print_clear_figure(PIXEL_0);
    fig_pos->y++;
    print_board();
  }
}

/**
 * @brief On ATTACHING state: add figure to field
 * @details If maximum level riched goes to GAMEOVER state
 */
static void on_attaching_state(void) {
  GameInfo_t *game = updateCurrentState();
  TetrisState_t *state = updateTetrisState();
  attach_figure_to_field();
  int n_rows = destruction_of_rows();
  recalculate_stats(n_rows);
  if (*state == ATTACHING && check_collide()) *state = SPAWN;
  if (game->level > MAX_LEVEL) *state = GAMEOVER;
  // *state = (check_collide()) ? GAMEOVER : SPAWN;
  // *state = (game->level > MAX_LEVEL || check_collide()) ? GAMEOVER : SPAWN;
  if (*state == SPAWN) print_board();
}

/**
 * @brief On GAMEOVER state: prints banner, awaits for input to quit
 *
 * @note USE_MOCK used for frontend stubs for unit tests of logic
 */
static void on_gameover_state(void) {
#ifndef USE_MOCK
  nodelay(stdscr, false);
  print_gameover_banner();
  getch();
#endif
}

/**
 * @brief On EXIT_ERROR state: prints banner, awaits for input to quit
 *
 * @note USE_MOCK used for frontend stubs for unit tests of logic
 */
static void on_exit_error_state(void) {
#ifndef USE_MOCK
  nodelay(stdscr, false);
  print_exit_error_banner();
  getch();
#endif
}

/**
 * @brief Processes user input and treturns action to be done based on it
 * @param[in] signal signal received from stdin (int)
 * @details Initial signal = 0. Then in the loop it is awaiting for timeout()
 * period of time for new input or proceed game state status accordingly
 */
UserAction_t get_action(int signal) {
  UserAction_t rc = No_signal;

  if (signal == KEY_UP)
    rc = Up;
  else if (signal == KEY_DOWN)
    rc = Down;
  else if (signal == KEY_LEFT)
    rc = Left;
  else if (signal == KEY_RIGHT)
    rc = Right;
  else if (signal == 'R' || signal == 'r')
    rc = Start;
  else if (signal == ' ')
    rc = Action;
  else if (signal == 'P' || signal == 'p')
    rc = Pause;
  else if (signal == ESCAPE)
    rc = Terminate;

  return rc;
}

/**
 * @brief Stub for unused in this game action
 */
static void __attribute__((unused)) moveup(void) {}

/**
 * @brief Moves the figure all the way down till it reaches field or border.
 * Updates figure output (prints 0 on initial position and 1 on new position)
 */
static void movedown(void) {
  FigurePos_t *fig_pos = updateFigurePosition();
  print_clear_figure(PIXEL_0);
  while (!check_collide()) fig_pos->y++;
  fig_pos->y--;
  print_clear_figure(PIXEL_1);
}

/**
 * @brief Moves the figure to the right if it will not cause collision.
 * Updates figure output (prints 0 on initial position and 1 on new position)
 */
static void moveright(void) {
  FigurePos_t *fig_pos = updateFigurePosition();
  print_clear_figure(PIXEL_0);
  fig_pos->x++;
  if (check_collide()) fig_pos->x--;
  print_clear_figure(PIXEL_1);
}

/**
 * @brief Moves the figure to the left if it will not cause collision.
 * Updates figure output (prints 0 on initial position and 1 on new position)
 */
static void moveleft(void) {
  FigurePos_t *fig_pos = updateFigurePosition();
  print_clear_figure(PIXEL_0);
  fig_pos->x--;
  if (check_collide()) fig_pos->x++;
  print_clear_figure(PIXEL_1);
}

/**
 * @brief Rotates the figure if it will not cause collision.
 * Updates figure output (prints 0 on initial position and 1 on new position)
 */
static void rotate_action(void) {
  int **figure = updateFigure();
  print_clear_figure(PIXEL_0);
  rotate_figure(figure);
  if (check_collide())
    for (int r = 0; r < 3; r++) rotate_figure(figure);
  print_clear_figure(PIXEL_1);
}

/**
 * @brief Pauses the game, awaits for input to quit
 *
 * @note USE_MOCK used for frontend stubs for unit tests of logic
 */
static void pause_game(void) {
#ifndef USE_MOCK
  GameInfo_t *game = updateCurrentState();
  nodelay(stdscr, false);
  print_pause_banner();
  getch();
  timeout(game->speed);
#endif
}
