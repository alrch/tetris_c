/**
 * @file tests.c
 * @brief Unit tests for Tetris game using Check library
 * @details This file contains comprehensive unit tests for the Tetris game
 * functionality, covering backend functions, finite state machine logic,
 * and game mechanics validation.
 */

#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/tetris.h"

// ===================
// TEST backend
// ===================

/**
 * @brief Test for updateCurrentState function consistency
 * @test Verifies that updateCurrentState returns the same pointer on multiple
 * calls
 * @pre Game should be initialized before calling updateCurrentState
 * @post Both calls should return identical pointer addresses
 */
START_TEST(test_updateCurrentState) {
  init_game();
  GameInfo_t *game1 = updateCurrentState();
  GameInfo_t *game2 = updateCurrentState();
  ck_assert_ptr_eq(game1, game2);
  free_game();
}
END_TEST

/**
 * @brief Test for updateFigure function consistency
 * @test Verifies that updateFigure returns the same pointer on multiple calls
 * @pre No specific initialization required
 * @post Both calls should return identical pointer addresses
 */
START_TEST(test_updateFigure) {
  int **figure1 = updateFigure();
  int **figure2 = updateFigure();
  ck_assert_ptr_eq(figure1, figure2);
  free_field(figure2);
}
END_TEST

/**
 * @brief Test for updateFigurePosition function consistency
 * @test Verifies that updateFigurePosition returns the same pointer on multiple
 * calls
 * @pre Figure position should be initialized before testing
 * @post Both calls should return identical pointer addresses
 */
START_TEST(test_updateFigurePosition) {
  init_figure_position();
  FigurePos_t *fig_pos1 = updateFigurePosition();
  FigurePos_t *fig_pos2 = updateFigurePosition();
  ck_assert_ptr_eq(fig_pos1, fig_pos2);
}
END_TEST

/**
 * @brief Test for assign_next_figure function
 * @test Verifies that assign_next_figure correctly initializes the next figure
 * @pre Game should be initialized and next figure array should be zeroed
 * @post Next figure should contain exactly 4 active pixels (standard tetromino)
 */
START_TEST(test_assign_next_figure) {
  init_game();
  GameInfo_t *game = updateCurrentState();
  int sum_of_pixels = 0;
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++)
      sum_of_pixels += game->next[i][j];
  ck_assert_int_eq(sum_of_pixels, 0);
  assign_next_figure();
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++)
      sum_of_pixels += game->next[i][j];
  ck_assert_int_eq(sum_of_pixels, 4);
  free_game();
}
END_TEST

/**
 * @brief Test for copy_next_figure_to_figure function
 * @test Verifies that copy_next_figure_to_figure correctly transfers next
 * figure to current figure
 * @pre Game should be initialized and next figure should be assigned
 * @post Current figure should match the next figure with exactly 4 active
 * pixels
 */
START_TEST(test_copy_next_figure_to_figure) {
  init_game();
  int **figure = updateFigure();
  int sum_of_pixels = 0;
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++)
      sum_of_pixels += figure[i][j];
  ck_assert_int_eq(sum_of_pixels, 0);
  assign_next_figure();
  copy_next_figure_to_figure();
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++)
      sum_of_pixels += figure[i][j];
  ck_assert_int_eq(sum_of_pixels, 4);
  free_field(figure);
  free_game();
}
END_TEST

/**
 * @brief Test for high score update functionality
 * @test Verifies that high score is properly updated and saved
 * @pre Game should be initialized with valid high score file
 * @post Game state should transition correctly based on file operations
 */
START_TEST(test_high_score_update) {
  TetrisState_t *state = updateTetrisState();
  GameInfo_t *game = updateCurrentState();
  int **figure = updateFigure();
  updateFigurePosition();
  init_game();
  *state = SPAWN;
  game->score = 100;
  game->high_score = 0;
  userInput(No_signal, false);
  FILE *record_note_r = fopen(HIGH_SCORE_FILE, "r+");
  if (record_note_r) {
    ck_assert_int_eq(*updateTetrisState(), MOVING);
    fclose(record_note_r);
  } else
    ck_assert_int_eq(*updateTetrisState(), EXIT_ERROR);

  free_field(figure);
  free_game();
}
END_TEST

/**
 * @brief Test for score recalculation
 * @test Different finishedrows give different score
 * @pre Game should be initialized
 * @post Scores should sum-up accordingly
 */
START_TEST(test_recalculate_stats) {
  TetrisState_t *state = updateTetrisState();
  GameInfo_t *game = updateCurrentState();
  updateFigurePosition();
  init_game();
  *state = ATTACHING;
  recalculate_stats(1);
  ck_assert_int_eq(game->score, 100);
  recalculate_stats(2);
  ck_assert_int_eq(game->score, 400);
  recalculate_stats(3);
  ck_assert_int_eq(game->score, 1100);
  recalculate_stats(4);
  ck_assert_int_eq(game->score, 2600);
  free_game();
}
END_TEST

/**
 * @brief Test for score recalculation
 * @test Different finishedrows give different score
 * @pre Game should be initialized
 * @post Scores should sum-up accordingly
 */
START_TEST(test_shift_rows_down) {
  TetrisState_t *state = updateTetrisState();
  GameInfo_t *game = updateCurrentState();
  updateFigurePosition();
  init_game();
  *state = ATTACHING;
  for (int i = 0; i < ROWS_MAP * COLS_MAP; i++)
    game->field[i / COLS_MAP][i % COLS_MAP] = 1;
  userInput(No_signal, false);
  int sum = 0;
  for (int i = 0; i < ROWS_MAP * COLS_MAP; i++)
    sum += game->field[i / COLS_MAP][i % COLS_MAP];
  userInput(No_signal, false);
  ck_assert_int_eq(sum, 0);
}
END_TEST

// ===================
// TEST FSM
// ===================

/**
 * @brief Test for START state transitions
 * @test Verifies correct state transitions from START state with different
 * inputs
 * @pre Game should be in START state
 * @post State should transition to SPAWN, GAMEOVER, or remain in START based on
 * input
 */
START_TEST(test_on_start_state) {
  updateCurrentState();
  init_game();
  TetrisState_t *state = updateTetrisState();

  *state = START;
  userInput(Start, false);
  ck_assert_int_eq(*updateTetrisState(), SPAWN);

  *state = START;
  userInput(Terminate, false);
  ck_assert_int_eq(*updateTetrisState(), GAMEOVER);

  *state = START;
  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), START);
  free_game();
}
END_TEST

/**
 * @brief Test for SPAWN state functionality
 * @test Verifies figure spawning and collision detection in SPAWN state
 * @pre Game should be in SPAWN state with proper initialization
 * @post State should transition to MOVING or GAMEOVER based on spawn conditions
 */
START_TEST(test_on_spawn_state) {
  TetrisState_t *state = updateTetrisState();
  GameInfo_t *game = updateCurrentState();
  int **figure = updateFigure();
  updateFigurePosition();

  init_game();
  *state = SPAWN;
  assign_next_figure();
  copy_next_figure_to_figure();
  init_figure_position();
  userInput(No_signal, false);

  FILE *record_note_r = fopen(HIGH_SCORE_FILE, "r+");
  if (record_note_r) {
    ck_assert_int_eq(*updateTetrisState(), MOVING);
    fclose(record_note_r);
  } else
    ck_assert_int_eq(*updateTetrisState(), EXIT_ERROR);

  *state = SPAWN;
  init_figure_position();
  for (int i = 0; i < ROWS_MAP * COLS_MAP; i++)
    game->field[i / COLS_MAP][i % COLS_MAP] = 1;
  userInput(No_signal, false);
  record_note_r = fopen(HIGH_SCORE_FILE, "r+");
  if (record_note_r) {
    ck_assert_int_eq(*updateTetrisState(), GAMEOVER);
    fclose(record_note_r);
  } else
    ck_assert_int_eq(*updateTetrisState(), EXIT_ERROR);

  free_field(figure);
  free_game();
}
END_TEST

/**
 * @brief Test for MOVING state transitions
 * @test Verifies all user input handling in MOVING state
 * @pre Game should be in MOVING state with active figure
 * @post State should transition correctly based on movement commands
 */
START_TEST(test_on_moving_state) {
  TetrisState_t *state = updateTetrisState();
  updateCurrentState();
  int **figure = updateFigure();
  FigurePos_t *fig_pos = updateFigurePosition();

  init_game();
  assign_next_figure();
  copy_next_figure_to_figure();
  init_figure_position();

  *state = MOVING;
  userInput(Up, false);
  ck_assert_int_eq(*updateTetrisState(), SHIFTING);
  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), MOVING);
  userInput(Right, false);
  ck_assert_int_eq(*updateTetrisState(), SHIFTING);
  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), MOVING);
  userInput(Left, false);
  ck_assert_int_eq(*updateTetrisState(), SHIFTING);
  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), MOVING);
  userInput(Action, false);
  ck_assert_int_eq(*updateTetrisState(), SHIFTING);
  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), MOVING);
  userInput(Pause, false);
  ck_assert_int_eq(*updateTetrisState(), SHIFTING);
  userInput(Pause, false);
  ck_assert_int_eq(*updateTetrisState(), MOVING);
  userInput(Down, false);
  ck_assert_int_eq(*updateTetrisState(), SHIFTING);
  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), ATTACHING);
  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), SPAWN);
  *state = MOVING;
  userInput(Terminate, false);
  ck_assert_int_eq(*updateTetrisState(), GAMEOVER);
  *state = MOVING;

  userInput(Action, false);
  ck_assert_int_eq(*updateTetrisState(), SHIFTING);
  // check when cannot rotate
  int stick_fig[16] = {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0};
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++)
      figure[i][j] = stick_fig[i * 4 + j];
  fig_pos->y = 1;
  free_field(figure);
  free_game();
}
END_TEST

/**
 * @brief Test for SHIFTING state functionality
 * @test Verifies figure shifting and collision detection
 * @pre Game should be in SHIFTING state with collision setup
 * @post State should transition to MOVING or ATTACHING based on collision
 */
START_TEST(test_on_shifting_state) {
  TetrisState_t *state = updateTetrisState();
  GameInfo_t *game = updateCurrentState();
  int **figure = updateFigure();
  updateFigurePosition();

  init_game();
  int stick_fig[16] = {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0};
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++)
      figure[i][j] = stick_fig[i * 4 + j];
  for (int i = 0; i < ROWS_MAP * COLS_MAP; i++)
    game->field[i / COLS_MAP][i % COLS_MAP] =
        (i < COLS_MAP * 5 && i % COLS_MAP) ? 0 : 1;
  *state = SHIFTING;
  init_figure_position();

  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), MOVING);

  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), SHIFTING);

  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), ATTACHING);

  free_field(figure);
  free_game();
}
END_TEST

/**
 * @brief Test for ATTACHING state functionality
 * @test Verifies figure attachment and next state transition
 * @pre Game should be in ATTACHING state with figure positioned for attachment
 * @post State should transition to SPAWN after successful attachment
 */
START_TEST(test_on_attaching_state) {
  TetrisState_t *state = updateTetrisState();
  GameInfo_t *game = updateCurrentState();
  int **figure = updateFigure();
  int stick_fig[16] = {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0};
  for (int i = 0; i < SIDE_OF_FIGURE_SQUARE; i++)
    for (int j = 0; j < SIDE_OF_FIGURE_SQUARE; j++)
      figure[i][j] = stick_fig[i * 4 + j];
  FigurePos_t *fig_pos = updateFigurePosition();

  init_game();
  assign_next_figure();
  copy_next_figure_to_figure();
  init_figure_position();
  for (int i = COLS_MAP * 5; i < ROWS_MAP * COLS_MAP; i++)
    game->field[i / COLS_MAP][i % COLS_MAP] = (i % COLS_MAP) ? 1 : 0;
  fig_pos->y = 1;
  *state = ATTACHING;
  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), SPAWN);

  free_field(figure);
  free_game();
}
END_TEST

/**
 * @brief Test for GAMEOVER state persistence
 * @test Verifies that GAMEOVER state remains unchanged
 * @pre Game should be in GAMEOVER state
 * @post State should remain in GAMEOVER regardless of input
 */
START_TEST(test_on_gameover_state) {
  TetrisState_t *state = updateTetrisState();
  *state = GAMEOVER;
  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), GAMEOVER);
}
END_TEST

/**
 * @brief Test for EXIT_ERROR state persistence
 * @test Verifies that EXIT_ERROR state remains unchanged
 * @pre Game should be in EXIT_ERROR state
 * @post State should remain in EXIT_ERROR regardless of input
 */
START_TEST(test_on_exit_error_state) {
  TetrisState_t *state = updateTetrisState();
  *state = EXIT_ERROR;
  userInput(No_signal, false);
  ck_assert_int_eq(*updateTetrisState(), EXIT_ERROR);
}
END_TEST

/**
 * @brief Test for get_action function key mapping
 * @test Verifies correct mapping of keyboard inputs to game actions
 * @pre No specific initialization required
 * @post All key mappings should return correct action codes
 */
START_TEST(test_get_action) {
  ck_assert_int_eq(get_action(KEY_UP), Up);
  ck_assert_int_eq(get_action(KEY_DOWN), Down);
  ck_assert_int_eq(get_action(KEY_LEFT), Left);
  ck_assert_int_eq(get_action(KEY_RIGHT), Right);
  ck_assert_int_eq(get_action('R'), Start);
  ck_assert_int_eq(get_action(' '), Action);
  ck_assert_int_eq(get_action('P'), Pause);
  ck_assert_int_eq(get_action(ESCAPE), Terminate);
}
END_TEST

/**
 * @brief Creates test suite with all test cases
 * @return Suite* Pointer to the created test suite
 * @details Groups all test cases into organized test suites for execution
 */
static Suite *create_tests(void) {
  Suite *s = suite_create("Basic Tests");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_updateCurrentState);
  tcase_add_test(tc_core, test_updateFigure);
  tcase_add_test(tc_core, test_updateFigurePosition);
  tcase_add_test(tc_core, test_assign_next_figure);
  tcase_add_test(tc_core, test_copy_next_figure_to_figure);
  tcase_add_test(tc_core, test_high_score_update);
  tcase_add_test(tc_core, test_recalculate_stats);
  tcase_add_test(tc_core, test_shift_rows_down);
  tcase_add_test(tc_core, test_on_start_state);
  tcase_add_test(tc_core, test_on_spawn_state);
  tcase_add_test(tc_core, test_on_moving_state);
  tcase_add_test(tc_core, test_on_shifting_state);
  tcase_add_test(tc_core, test_on_attaching_state);
  tcase_add_test(tc_core, test_on_gameover_state);
  tcase_add_test(tc_core, test_on_exit_error_state);
  tcase_add_test(tc_core, test_get_action);
  suite_add_tcase(s, tc_core);
  return s;
}

/**
 * @brief Main test runner function
 * @return int EXIT_SUCCESS if all tests pass, EXIT_FAILURE otherwise
 * @details Executes all test suites and reports results
 */
int main(void) {
  int number_failed;
  Suite *s = create_tests();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  sr = NULL;

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}