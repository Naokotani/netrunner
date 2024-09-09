#include "netrunner.h"
#include <curses.h>
#include <stdio.h>

void *allocate_array(size_t nbem, size_t size) {
  void *ptr;
  if ((ptr =
       calloc(nbem, size)) == NULL)
    err(1, "Not enough memory");
  return ptr;
}

void *allocate_struct(size_t size) {
  void *ptr;
  if ((ptr = malloc(size)) == NULL)
    err(1, "Not enough memory");
  return ptr;
}

int build_ui(unsigned height, unsigned width, Position *pos, Grid_state *grid_state) {
  clear();
  WINDOW *menu = newwin(getmaxy(stdscr), 20, 0, 0);
  Point center = center_grid(height, width);

  unsigned first_row = center.y;
  unsigned first_col = center.x;
  unsigned last_row = (height - 1) * 2 + first_row;
  unsigned last_col = (width - 1) * 4 + first_col;

  unsigned i = 0;
  for (unsigned row = first_row; row <= last_row; row += 2) {
    for (unsigned col = first_col; col <= last_col; col += 4) {
      refresh();
      WINDOW *win = newwin(GRID_HEIGHT, GRID_WIDTH, row, col);
      if (row == first_row && col == first_col) {
        top_left_box(win);
        grid_state[i].grid_pos = TL;
      } else if (row == first_row && col != last_col) {
        top_box(win);
        grid_state[i].grid_pos = T;
      } else if (row == first_row && col == last_col) {
        top_right_box(win);
        grid_state[i].grid_pos = TR;
      } else if (row != first_row && col == first_col && row != last_row) {
        left_box(win);
        grid_state[i].grid_pos = L;
      } else if (row != first_row && col != first_col &&
                 row != last_row && col != last_col) {
        mid_box(win);
        grid_state[i].grid_pos = M;
      } else if (row != first_row && row != last_row && col == last_col) {
        right_box(win);
        grid_state[i].grid_pos = R;
      } else if (row == last_row && col == first_col) {
        bottom_left_box(win);
        grid_state[i].grid_pos = BL;
      } else if (row == last_row && col != last_col) {
        bottom_box(win);
        grid_state[i].grid_pos = B;
      } else if (row == last_row && col == last_col) {
        bottom_right_box(win);
        grid_state[i].grid_pos = BR;
      }

      if (grid_state[i].is_symbol) {
        print_char(win, 1, 2, grid_state[i].code);
      }

      if (grid_state[i].is_wall) {
        create_wall(pos, win, get_proximal_states(pos, row, col, grid_state));
      }

      wrefresh(win);
      pos->grid[i] = win;
      i++;
    }
  }

  Menu_state menu_state = GRID;

  return main_loop(menu_state, pos,
                   grid_state,  menu); 
}

int main() {
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  keypad(stdscr, true);
  start_color();
  init_pair(1, COLOR_MAGENTA, COLOR_MAGENTA);

  unsigned height = 10;
  unsigned width = 10;
  unsigned size = height*width;

  Grid_state *grid_state = allocate_array(size, sizeof(Grid_state));

  for (size_t i = 0; i < size; i++) {
    grid_state[i] = (Grid_state) {
          E, false,
          false,"","" };
  }

  Position *pos = allocate_struct(sizeof(Position));
  *pos->grid = allocate_array(size, sizeof(WINDOW *));

  pos->height = height;
  pos->width = width;
  pos->curr_col = 1;
  pos->curr_row = 1;

  return build_ui(height, width, pos, grid_state);

  endwin();
  return 0;
}
