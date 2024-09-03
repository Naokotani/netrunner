#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <err.h>
#include <stdbool.h>
#include <curses.h>
#define GRID_HEIGHT 3
#define GRID_WIDTH 5

typedef struct {
  int y;
  int x;
} Point;

typedef enum  {
  ADD,
  GRID,
  SELECT,
  WALL,
  EXIT
} Menu_state;

typedef enum {
  TL,
  T,
  TR,
  L,
  M,
  R,
  BR,
  B,
  BL,
  E
}Grid_pos;

typedef struct {
  Grid_pos grid_pos;
  bool is_wall;
}Grid_state;

typedef struct {
  Grid_state left;
  Grid_state right;
  Grid_state up;
  Grid_state down;
  Grid_state center;
}Proximal_states;

typedef struct {
  char *code;
  char *label;
} Netcode;

typedef struct {
  size_t size;
  Netcode *array[];
}Netcode_array;

typedef struct {
  unsigned curr_row;
  unsigned curr_col;
  unsigned width;
  unsigned height;
  WINDOW *grid[];
} Position;

// main.c
Menu_state net_menu(Position *pos, WINDOW *win, Netcode *netcodes[], size_t menu_len);
int main_loop(Menu_state menu_state, Position *pos, Grid_state *grid_state,
              Netcode *netcodes[], size_t net_menu_len, WINDOW* menu);
int build_ui();

// move.c
Point center_grid(size_t height, size_t width);
Point center_horizontal(WINDOW *win, char * string, unsigned height);
WINDOW *get_pos(Position *pos, size_t row, size_t col);
WINDOW *print_center_char(Position *pos, char* ch, size_t row, size_t col);
void print_char(WINDOW *win, unsigned row, unsigned col, char* ch);
Position *move_cur(Position *pos, size_t row, size_t col);
Menu_state grid_movement(Position *pos);

// wall.c
Proximal_states get_proximal_states(Position *pos, Grid_state *grid_state);
void create_wall(Position *pos, Proximal_states p_states, WINDOW *win);
void clear_wall(Position *pos, Proximal_states p_states, Grid_state *grid_state);
Menu_state create_wall_mode(Position *pos, Grid_state *grid_state);

// menu.c
void create_menu(WINDOW *win, Netcode_array *netcodes);
void add_menu(WINDOW *win, Netcode *netcodes );


// box.c
void top_left_box(WINDOW *win);
void top_box(WINDOW *win);
void top_right_box(WINDOW *win);
void left_box(WINDOW *win);
void right_box(WINDOW *win);
void mid_box(WINDOW *win);
void bottom_left_box(WINDOW *win);
void bottom_box(WINDOW *win);
void bottom_right_box(WINDOW *win);
