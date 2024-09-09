#include "netrunner.h"
#include <curses.h>

Proximal_states get_proximal_states(Position *pos, unsigned row, unsigned col, Grid_state *grid_state) {
  Grid_state left;
  Grid_state right;
  Grid_state up;
  Grid_state down;
  Grid_state center = grid_state[(row - 1) * pos->width + (col - 1)];

  if (col > 1) {
    left = grid_state[(row - 1) * pos->width + (col - 2)];
  } else {
    left = (Grid_state){E, false, false, "", ""};
  }

  if (col < pos->width) {
    right = grid_state[(row - 1) * pos->width + (col)];
  } else {
    right = (Grid_state){E, false, false, "", ""};
  }

  if (row > 1) {
    up = grid_state[(row - 2) * pos->width + (col - 1)];
  } else {
    up = (Grid_state){E, false, false, "", ""};
  }

  if (row < pos->height) {
    down = grid_state[(row) * pos->width + (col - 1)];
  } else {
    down = (Grid_state){E, false, false, "", ""};
  }

  return (Proximal_states) {
    left, right, up, down, center};
}

void create_wall(Position *pos, WINDOW* win, Proximal_states p_states) {
    for (int c = 1; c < GRID_WIDTH - 1; c++) {
      if (p_states.up.is_wall == true) {
        mvwaddstr(win, 0, c, "\u2588");
      } else {
        mvwaddstr(win, 0, c, "\u2584");
      }

      if (p_states.down.is_wall == true) {
        mvwaddstr(win, 2, c, "\u2588");
      } else {
        mvwaddstr(win, 2, c, "\u2580");
      }
    }

    if (p_states.left.is_wall == true) {
      mvwaddstr(win, 1, 0, "\u2588");
    } else {
      mvwaddstr(win, 1, 0, "\u2590");
    }

    if (p_states.right.is_wall == true) {
      mvwaddstr(win, 1, 4, "\u2588");
    } else {
      mvwaddstr(win, 1, 4, "\u258C");
    }

    for (int c = 1; c < GRID_WIDTH - 1; c++) {
      mvwaddstr(win, 1, c, "\u2588");
    }

    move_cur(pos, pos->curr_row, pos->curr_col);
}

void clear_wall(Position *pos, Proximal_states p_states, Grid_state *grid_state) {
  curs_set(0);
  WINDOW * win = get_pos(pos, pos->curr_row, pos->curr_col);
  wclear(win);
  Grid_pos grid_pos = p_states.center.grid_pos;
  switch (grid_pos) {
  case TL:
    top_left_box(win);
    print_char(win, 2, 0, "\u251C");
    print_char(win, 0, 4, "\u252C");
    print_char(win, 2, 4, "\u253C");
    break;
  case T:
    top_box(win);
    print_char(win, 0, 4, "\u252C");
    print_char(win, 2, 4, "\u253C");
    print_char(win, 2, 0, "\u253C");
    break;
  case TR:
    top_right_box(win);
    print_char(win, 2, 4, "\u2524");
    print_char(win, 2, 0, "\u253C");
    break;
  case L:
    left_box(win);
    print_char(win, 2, 0, "\u251C");
    print_char(win, 0, 4, "\u253C");
    print_char(win, 2, 4, "\u253C");
    break;
  case R:
    right_box(win);
    print_char(win, 2, 4, "\u2524");
    print_char(win, 2, 0, "\u253C");
    break;
  case M:
    mid_box(win);
    print_char(win, 2, 0, "\u253C");
    print_char(win, 0, 4, "\u253C");
    print_char(win, 2, 4, "\u253C");
    break;
  case BL:
    bottom_left_box(win);
    print_char(win, 0, 4, "\u253C");
    print_char(win, 2, 4, "\u2534");
    break;
  case B:
    bottom_box(win);
    print_char(win, 2, 4, "\u2534");
    print_char(win, 0, 4, "\u253C");
    break;
  case BR:
    bottom_right_box(win);
    break;
  case E:
    break;
  }

  wrefresh(win);

  for (int col = 1; col < GRID_WIDTH - 1; col++) {
    print_char(win, 2, col, "\u2500");
  }

  print_char(win, 1, 4, "\u2502");

  Proximal_states rebuild_prox_states;
  if (p_states.up.is_wall == true) {
    move_cur(pos, pos->curr_row - 1, pos->curr_col);
    rebuild_prox_states = get_proximal_states(pos, pos->curr_row, pos->curr_col, grid_state);
    create_wall(pos, win, rebuild_prox_states);
    move_cur(pos, pos->curr_row + 1, pos->curr_col);
  }

  if (p_states.down.is_wall == true) {
    move_cur(pos, pos->curr_row + 1, pos->curr_col);
    rebuild_prox_states = get_proximal_states(pos, pos->curr_row, pos->curr_col, grid_state);
    create_wall(pos, win, rebuild_prox_states);
    move_cur(pos, pos->curr_row - 1, pos->curr_col);
  }

  if (p_states.left.is_wall == true) {
    move_cur(pos, pos->curr_row, pos->curr_col - 1);
    rebuild_prox_states = get_proximal_states(pos, pos->curr_row, pos->curr_col, grid_state);
    create_wall(pos, win, rebuild_prox_states);
    move_cur(pos, pos->curr_row, pos->curr_col + 1);
  }

  if (p_states.right.is_wall == true) {
    move_cur(pos, pos->curr_row, pos->curr_col + 1);
    rebuild_prox_states = get_proximal_states(pos, pos->curr_row, pos->curr_col, grid_state);
    create_wall(pos, win, rebuild_prox_states);
    move_cur(pos, pos->curr_row, pos->curr_col - 1);
  }

  curs_set(1);
  move_cur(pos, pos->curr_row, pos->curr_col);
}

Menu_state create_wall_mode(Position *pos, Grid_state *grid_state) {
  int choice = '\n';
  while (1) {
    Proximal_states p_states = get_proximal_states(pos, pos->curr_row, pos->curr_col, grid_state);

    switch (choice) {
    case KEY_UP:
      if (pos->curr_row > 1) {
        pos = move_cur(pos, pos->curr_row - 1,
                            pos->curr_col);
      }
      break;
    case KEY_LEFT:
      if (pos->curr_col > 1) {
        pos= move_cur(pos, pos->curr_row,
                            pos->curr_col - 1);
      }
      break;
    case KEY_DOWN:
      if (pos->curr_row < pos->height) {
        pos= move_cur(pos, pos->curr_row + 1,
                            pos->curr_col);
      }
      break;
    case KEY_RIGHT:
      if (pos->curr_col < pos->width) {
        pos= move_cur(pos, pos->curr_row,
                            pos->curr_col + 1);
      }
      break;
    case '\n':
      if (grid_state[(pos->curr_row - 1) * pos->width +
                     (pos->curr_col - 1)].is_wall) {
        grid_state[(pos->curr_row - 1) * pos->width +
                   (pos->curr_col - 1)].is_wall = false;
        clear_wall(pos, p_states, grid_state);
      } else {
        grid_state[(pos->curr_row - 1) * pos->width +
                   (pos->curr_col - 1)].is_wall = true;
        create_wall(pos, get_pos(pos, pos->curr_row, pos->curr_col), p_states);
      }
      break;
    case 'q':
      return (Menu_state) SELECT;
    };
    choice = getch();
  }
  return (Menu_state) GRID;
}
