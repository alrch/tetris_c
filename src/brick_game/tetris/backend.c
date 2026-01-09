/**
 * @file backend.c
 * @brief Functions used for logic of the game
 * @details This file implements logic of game functions with staic objects game
 * info and figure and its position on the game field
 */

#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../include/tetris.h"

static int init_field(int ***field, int rows, int cols);
static void figures_choice(int **figure, int n);
static int check_finished_row(int const *row);
static void shift_rows_down(int row);

/**
 * @brief update game info. Keep static variable of game info, GameInfo_t
 *
 * @return pointer to game info
 */
GameInfo_t *updateCurrentState(void) {
  static GameInfo_t game = {0};
  return &game;
}

/**
 * @brief update figure. Keep static variable of figure, int **
 *
 * @return pointer to figure
 */
int **updateFigure(void) {
  static int **figure = NULL;
  if (figure == NULL)
    init_field(&figure, SIDE_OF_FIGURE_SQUARE, SIDE_OF_FIGURE_SQUARE);
  return figure;
}

/**
 * @brief update figure position. Keep static variable of figure positon,
 * FigurePos_t
 *
 * @return pointer to figure position
 */
FigurePos_t *updateFigurePosition(void) {
  static FigurePos_t fig_pos = {0};
  return &fig_pos;
}

/**
 * @brief Main game loop that controls the game flow
 * @details Manages game initialization, state transitions, user input
 * processing, and game cleanup. Handles different game states (START, MOVING,
 * GAMEOVER, etc.) and ensures proper resource cleanup on exit.
 *
 * The loop continues until the game reaches GAMEOVER or EXIT_ERROR state.
 * In MOVING and START states, it captures user input for game control.
 *
 * @note USE_MOCK used for frontend stubs for unit tests of logic
 */
void game_loop(void) {
  int continue_flag = true;
  int signal = 0;

  TetrisState_t *state = updateTetrisState();
  while (continue_flag) {
    if (*state == GAMEOVER || *state == EXIT_ERROR) continue_flag = false;
    userInput(get_action(signal), false);
#ifndef USE_MOCK
    if (*state == MOVING || *state == START) signal = getch();
#endif
  }
  if (*state == EXIT_ERROR) {
    print_exit_error_banner();
#ifndef USE_MOCK
    nodelay(stdscr, false);
    getch();
#endif
  }
}

/**
 * @brief initialise game structure with values, initialise field and next
 * figure (memory allocation for matrices x2). Update game info
 *
 * @return error code
 */
int init_game(void) {
  srand(
      time(NULL)); /**< Seed random number generator for figure randomization */
#ifndef USE_MOCK
  NCURSES_INIT(-1);      /**< Initialize ncurses window with default settings */
  setlocale(LC_ALL, ""); /**< Set locale for international character support */
  print_overlay();       /**< Display initial game frame and intro message */
#endif
  int error = NO_ERROR;
  int **figure = updateFigure();
  if (figure == NULL) error = ERROR;
  GameInfo_t *game = updateCurrentState();
  TetrisState_t *state = updateTetrisState();
  *state = (error == NO_ERROR) ? START : EXIT_ERROR;
  if (error == NO_ERROR) error = init_field(&(game->field), ROWS_MAP, COLS_MAP);
  if (error == NO_ERROR)
    error =
        init_field(&(game->next), SIDE_OF_FIGURE_SQUARE, SIDE_OF_FIGURE_SQUARE);
  game->score = 0;
  game->level = 1;
  game->high_score = 0;
  game->speed = INITIAL_TIMEOUT;
  game->pause = 0;
  return error;
}

/**
 * @brief Frees resourses, exit ncurses,
 *
 * @note USE_MOCK used for frontend stubs for unit tests of logic
 */
void exit_game() {
#ifndef USE_MOCK
  endwin(); /**< Clean up ncurses resources */
#endif
  int **figure = updateFigure();
  free_game();
  free_field(figure);
}
/**
 * @brief initialize matrix x 2 of rows * cols
 *
 * @param[in] field pointer to matrix x 2 (int ***)
 * @param[in] rows number of rows
 * @param[in] cols number of columns
 * @return error code
 */
static int init_field(int ***field, int rows, int cols) {
  int error = ERROR;
  *field = calloc(rows, sizeof(int *));
  if (*field) {
    (*field)[0] = calloc(rows * cols, sizeof(int));
    if ((*field)[0]) {
      error = NO_ERROR;
      for (int i = 0; i < rows; i++) (*field)[i] = (*field)[0] + i * cols;
    } else {
      free(*field);
      *field = NULL;
    }
  }
  return error;
}

/**
 * @brief free memory allocated for matrix x 2
 *
 * @param[in] field matrix x 2 (int **)
 * @return error code
 */
void free_field(int **field) {
  if (field && field[0]) free(field[0]);
  if (field) free(field);
}

/**
 * @brief free game, free memory allocated for field and next figure. Update
 * game info
 */
void free_game(void) {
  GameInfo_t *game = updateCurrentState();
  free_field(game->field);
  free_field(game->next);
}

/**
 * @brief free game, free memory allocated for field and next figure
 */
void assign_next_figure(void) {
  GameInfo_t *game = updateCurrentState();
  int random = rand();
  int rotation_n = random % 4;
  int fig_number = random % NUMBER_OF_FIGURES;
  figures_choice(game->next, fig_number);
  for (int r = 0; r < rotation_n; r++) rotate_figure(game->next);
}

/**
 * @brief copy next figure to current figure. Update game info wit next figure
 */
void copy_next_figure_to_figure(void) {
  GameInfo_t *game = updateCurrentState();
  int **figure = updateFigure();
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++)
      figure[i][j] = game->next[i][j];
}

/**
 * @brief update high score in game info: read from HIGH_SCORE_FILE and
 * compare with current score. Update both game info and file
 *
 * @return error code
 */
int high_score_update(void) {
  GameInfo_t *game = updateCurrentState();
  int rc = NO_ERROR;
  if (game->high_score == 0 || game->score > game->high_score) {
    FILE *record_note_r = fopen(HIGH_SCORE_FILE, "r+");
    if (record_note_r) {
      fscanf(record_note_r, "%d", &game->high_score);
      if (game->score > game->high_score) {
        rewind(record_note_r);
        fprintf(record_note_r, "%d", game->score);
        game->high_score = game->score;
      }
      fclose(record_note_r);
    } else {
      FILE *record_note_w = fopen(HIGH_SCORE_FILE, "w");
      if (record_note_w) {
        fprintf(record_note_w, "%d", game->score);
        fclose(record_note_w);
      } else
        rc = ERROR;
    }
  }
  return rc;
}

/**
 * @brief initialize start figure position to attach to the middle of "ceiling"
 * of field. Update figure position.
 */
void init_figure_position(void) {
  int **figure = updateFigure();
  FigurePos_t *fig_pos = updateFigurePosition();
  fig_pos->x = FIGURESTART_X;
  fig_pos->y = FIGURESTART_Y;
  for (int i = 0, sum = 0; sum == 0 && i < SIDE_OF_FIGURE_SQUARE; i++) {
    for (int j = 0; sum == 0 && j < SIDE_OF_FIGURE_SQUARE; j++)
      sum += figure[i][j];
    if (sum == 0) fig_pos->y--;
  }
  for (int i = 0, sum = 0; sum == 0 && i < SIDE_OF_FIGURE_SQUARE; i++) {
    for (int j = 0; sum == 0 && j < SIDE_OF_FIGURE_SQUARE; j++)
      sum += figure[j][i];
    if (sum == 0) fig_pos->x--;
  }
}

/**
 * @brief check if some rows are finished. Call their destruction and shift
 * field down. Update game info field
 *
 * @return amount of finished rows
 */
int destruction_of_rows(void) {
  GameInfo_t *game = updateCurrentState();
  int n_rows = 0;
  for (int i = 0; i < ROWS_MAP; i++)
    if (check_finished_row(game->field[i])) {
      n_rows++;
      shift_rows_down(i);
    }
  return n_rows;
}

/**
 * @brief check if the row is finished
 * @param[in] row pointer to row of matrix (int *)
 * @return amount of finished rows
 */
static int check_finished_row(int const *row) {
  int finished_row = 1;
  for (int j = 0; finished_row == 1 && j < COLS_MAP; j++)
    if (row[j] != 1) finished_row = 0;
  return finished_row;
}

/**
 * @brief aemove finished row: shifts all upper rows down. Update game info
 * field
 * @param[in] row number of row
 */
static void shift_rows_down(int row) {
  GameInfo_t *game = updateCurrentState();
  for (int i = row; i >= 0; i--) {
    for (int j = 0; j < COLS_MAP; j++)
      game->field[i][j] = (i > 0) ? game->field[i - 1][j] : 0;
  }
}

/**
 * @brief check if the position/rotation of figure collides with field or
 * borders
 *
 * @return error code
 */
int check_collide(void) {
  GameInfo_t *game = updateCurrentState();
  FigurePos_t *fig_pos = updateFigurePosition();
  int **figure = updateFigure();
  int rc = false;
  for (int i = 0; rc == false && i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; rc == false && j < SIDE_OF_FIGURE_SQUARE; j++)
      if (figure[i][j] == 1) {
        int y = fig_pos->y + i;
        int x = fig_pos->x + j;
        if (x < 0 || y < 0 || x >= COLS_MAP || y >= ROWS_MAP ||
            game->field[y][x] == 1) {
          rc = true;
        }
      }
  return rc;
}

/**
 * @brief update game info score and increments speed based on number of rows
 * finished and destroyed
 * @param[in] n_rows amount of rows
 */
void recalculate_stats(int n_rows) {
  GameInfo_t *game = updateCurrentState();
  if (n_rows) {
    if (n_rows == 1) game->score += 100;
    if (n_rows == 2) game->score += 300;
    if (n_rows == 3) game->score += 700;
    if (n_rows == 4) game->score += 1500;
    game->level = 1 + game->score / 600;
    game->speed = INITIAL_TIMEOUT - (game->level - 1) * SPEED_DECREMENT;
  }
}

/**
 * @brief rotate figure clockwise, updates static figure
 * @param[in] figure pointer to figure matrix (int **)
 */
void rotate_figure(int **figure) {
  int tmp_figure[SIDE_OF_FIGURE_SQUARE][SIDE_OF_FIGURE_SQUARE];
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++)
      tmp_figure[i][j] = figure[j][SIDE_OF_FIGURE_SQUARE - i - 1];
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++)
      figure[i][j] = tmp_figure[i][j];
}

/**
 * @brief add 1 values of figure matrix on certain coordinates to field matrix 1
 * values. Update game info field
 */
void attach_figure_to_field(void) {
  int **figure = updateFigure();
  GameInfo_t *game = updateCurrentState();
  FigurePos_t *fig_pos = updateFigurePosition();
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++) {
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++) {
      int y = fig_pos->y + i;
      int x = fig_pos->x + j;
      if (figure[i][j]) game->field[y][x] = 1;
    }
  }
}

/**
 * @brief array[amount of figures][rows][cols] of available figures. Assign n-th
 * figure from array to figure
 * @param[in] figure pointer to figure matrix (int **)
 * @param[in] n number of figure
 */
static void figures_choice(int **figure, int n) {
  int all_figures[NUMBER_OF_FIGURES][SIDE_OF_FIGURE_SQUARE]
                 [SIDE_OF_FIGURE_SQUARE] = {
                     {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
                     {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
                     {{0, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
                     {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
                     {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
                     {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
                     {{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}};
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++)
      figure[i][j] = all_figures[n][i][j];
}
