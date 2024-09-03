#include "netrunner.h"
#define MENU_LEN 4

int main_loop(Menu_state menu_state, Position *pos, Grid_state *grid_state,
              Netcode *netcodes[], size_t net_menu_len, WINDOW* menu){
  while (1) {
    switch (menu_state) {
    case GRID:
      menu_state = grid_movement(pos);
      break;
    case ADD:
      break;
    case SELECT:
      curs_set(0);
      menu_state = net_menu(pos, menu, netcodes, net_menu_len);
      curs_set(1);
      move_cur(pos, pos->curr_row, pos->curr_col);
      break;
    case EXIT:
      free(pos);
      endwin();
      return 0;
    case WALL:
      menu_state = create_wall_mode(pos, grid_state);
      break;
    }
  }
  return 0;
}

int build_ui() {
  unsigned width = 30;
  unsigned height = 20;
  
  Grid_state *grid_state;
  if ((grid_state =
       reallocarray(NULL, height * width, sizeof(Grid_state))) == NULL)
    err(1, "Grid state realloc");

  Position *position;
  if ((position = malloc(sizeof(Position))) == NULL)
    err(1, "position malloc");
  if ((*position->grid =
           reallocarray(NULL, height * width, sizeof(WINDOW *))) == NULL)
    err(1, "position realloc");

  position->height = height;
  position->width = width;
  position->curr_col = 1;
  position->curr_row = 1;

  size_t net_menu_len = MENU_LEN;
  Netcode codes[MENU_LEN] = {
    {"\u2588", "Wall Mode"},
    {"=", "Gate 2"},
    {"\u2261", "Gate 3"},
    {"\u2263", "Gate 4"}};

  Netcode_array *netcodes;
  if ((netcodes = malloc(sizeof(Netcode_array))) == NULL)
    err(1, "netcode malloc");

  if ((netcodes = reallocarray(NULL, MENU_LEN, sizeof(Netcode)))
      ==NULL)
    err(1, "netcodes reallocarray");

  netcodes->size = MENU_LEN;

  for (size_t i = 0; i < MENU_LEN; i++) {
    netcodes->array[i] = &codes[i];
  }

  WINDOW *menu = newwin(getmaxy(stdscr), 20, 0, 0);
  create_menu(menu, netcodes);

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
        grid_state[i] = (Grid_state){TL, false};
      } else if (row == first_row && col != last_col) {
        top_box(win);
        grid_state[i] = (Grid_state){T, false};
      } else if (row == first_row && col == last_col) {
        top_right_box(win);
        grid_state[i] = (Grid_state){TR, false};
      } else if (row != first_row && col == first_col && row != last_row) {
        left_box(win);
        grid_state[i] = (Grid_state){L, false};
      } else if (row != first_row && col != first_col &&
                 row != last_row && col != last_col) {
        mid_box(win);
        grid_state[i] = (Grid_state){M, false};
      } else if (row != first_row && row != last_row && col == last_col) {
        right_box(win);
        grid_state[i] = (Grid_state){R, false};
      } else if (row == last_row && col == first_col) {
        bottom_left_box(win);
        grid_state[i] = (Grid_state){BL, false};
      } else if (row == last_row && col != last_col) {
        bottom_box(win);
        grid_state[i] = (Grid_state){B, false};
      } else if (row == last_row && col == last_col) {
        bottom_right_box(win);
        grid_state[i] = (Grid_state){BR, false};
      }
      position->grid[i] = win;
      i++;
    }
  }

  position = move_cur(position, 1, 1);
  Menu_state menu_state = GRID;

  return main_loop(menu_state, position,
                   grid_state, netcodes->array, net_menu_len, menu); 
}

int main() {
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  keypad(stdscr, true);
  start_color();
  init_pair(1, COLOR_MAGENTA, COLOR_MAGENTA);

  return build_ui();

  endwin();
  return 0;
}
