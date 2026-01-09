/**
 * @file mock_ncurses.h
 * @brief Mock ncurses header for unit testing
 */

#ifndef MOCK_NCURSES_H
#define MOCK_NCURSES_H

/**
 * @brief Mock WINDOW structure for testing
 */
typedef struct {
  int width;  /**< Mock window width */
  int height; /**< Mock window height */
} WINDOW;

extern WINDOW mock_win;
extern WINDOW *stdscr;

/* Frontend function declarations */
void print_overlay(void);
void clear_and_print_next_figure(void);
void print_board(void);
void print_pause_banner(void);
void print_gameover_banner(void);
void print_exit_error_banner(void);
void print_stats(void);
void print_clear_figure(char *tray);

/* Mock ncurses function declarations */
int wgetch(WINDOW *win);
void wtimeout(WINDOW *win, int delay);
WINDOW *initscr(void);
int endwin(void);
void noecho(void);
int curs_set(int visibility);
int keypad(WINDOW *win, int enable);

/* Mock constants */
#define OK 0

#endif /* MOCK_NCURSES_H */
