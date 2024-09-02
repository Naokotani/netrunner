#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
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
}Proximal_states;

typedef struct {
  char *code;
  char *label;
} Netcode;

typedef struct {
  int curr_row;
  int curr_col;
  size_t width;
  size_t height;
  WINDOW *grid[];
} Position;

Point center_grid(size_t height, size_t width) {
  unsigned x = getmaxx(stdscr);
  unsigned y = getmaxy(stdscr);
  Point center = {(y / 2) - height, (x / 2) - width * 2};
  return center;
}

Point center_horizontal(WINDOW *win, char * string, unsigned height) {
  Point c;
  unsigned content_width = strlen(string);
  getmaxyx(win, c.y, c.x);
  c.x = c.x / 2 - content_width / 2;
  c.y = height;
  mvwprintw(win, c.y, c.x, "%s", string);
  wrefresh(win);
  return c;
}

void print_cursor_position() {
  int row;
  int col;
  getyx(stdscr, row, col);
  printw("Cursor position is %d %d\n", row, col);
  refresh();
}

void top_left_box(WINDOW *win) {
  wborder(win, ACS_VLINE, ' ',
          ACS_HLINE, ' ' , ACS_ULCORNER, ' ',
          ' ', ' ');
  wrefresh(win);
}

void top_right_box(WINDOW *win) {
  wborder(win, ACS_VLINE, ACS_VLINE,
          ACS_HLINE, ' ', ACS_TTEE, ACS_URCORNER,
          ' ', ' ');
  wrefresh(win);
}

void bottom_left_box(WINDOW *win) {
  wborder(win, ACS_VLINE, ' ',
          ACS_HLINE, ACS_HLINE, ACS_LTEE, ' ',
          ACS_LLCORNER, ' ');
  wrefresh(win);
}

void bottom_right_box(WINDOW *win) {
  refresh();
  wborder(win, ACS_VLINE, ACS_VLINE,
          ACS_HLINE, ACS_HLINE, ACS_PLUS, ACS_RTEE,
          ACS_BTEE, ACS_LRCORNER);
  wrefresh(win);
}

void top_box(WINDOW *win) {
  wborder(win, ACS_VLINE, ' ',
          ACS_HLINE, ' ', ACS_TTEE, ' ',
          ' ', ' ');
  wrefresh(win);
}

void mid_box(WINDOW *win) {
    wborder(win, ACS_VLINE, ' ',
            ACS_HLINE, ' ', ACS_PLUS, ' ',
            ' ' , ' ');
    wrefresh(win);
}

void left_box(WINDOW *win) {
    wborder(win, ACS_VLINE, ' ',
            ACS_HLINE, ' ', ACS_LTEE, ' ',
            ' ', ' ');
    wrefresh(win);
}

void right_box(WINDOW *win) {
    wborder(win, ACS_VLINE, ACS_VLINE,
            ACS_HLINE, ' ', ACS_PLUS, ACS_RTEE,
            ' ', ' ');
    wrefresh(win);
}

void bottom_box(WINDOW *win) {
    wborder(win, ACS_VLINE, ' ',
            ACS_HLINE, ACS_HLINE, ACS_PLUS, ' ',
            ACS_BTEE, ' ');
    wrefresh(win);
}

WINDOW *get_pos(Position *pos, size_t row, size_t col) {
  WINDOW *win = pos->grid[(row - 1) * pos->width + (col - 1)];
  return win;
}

WINDOW *print_char(Position *pos, char* ch, size_t row, size_t col) {
  WINDOW *win = get_pos(pos, row, col);
  mvwprintw(win, 1, 2, "%s", ch);
  wrefresh(win);
  return win;
}

Position *move_cur(Position *pos, size_t row, size_t col) {
  WINDOW *win = get_pos(pos, row, col);
  wmove(win, 1, 2);
  wrefresh(win);
  pos->curr_col = col;
  pos->curr_row = row;
  return pos;
}

Menu_state net_menu(Position *pos, WINDOW *win, Netcode netcodes[], size_t menu_len) {
  unsigned highlight = 0;
  int choice;
  while (1) {
    for (size_t i = 0; i < menu_len; i++) {
      if (i == highlight) {
        wattron(win, A_REVERSE);
        mvwprintw(win, i + 5, 2, "%s %s", netcodes[i].code, netcodes[i].label);
        wattroff(win, A_REVERSE);
      } else {
        mvwprintw(win, i + 5, 2, "%s %s", netcodes[i].code, netcodes[i].label);
      }
    }
    wrefresh(win);
    choice = getch();
    
    switch (choice) {
    case KEY_UP:
      if (highlight > 0) {
        highlight--;
      }
      break;
    case KEY_DOWN:
      if (highlight < menu_len - 1) {
        highlight++;
      }
      break;
    case '\n':
      for (size_t i = 0; i < menu_len ; i++) {
        mvwprintw(win, i + 5, 2, "%s %s", netcodes[i].code, netcodes[i].label);
      }
      wrefresh(win);
      if (highlight == 0) {
        return (Menu_state) WALL;
      } else {
        print_char(pos, netcodes[highlight].code,
                   pos->curr_row, pos->curr_col);
        return (Menu_state) GRID;
      }
      break;
    case 'q':
      for (size_t i = 0; i < menu_len ; i++) {
        mvwprintw(win, i + 5, 2, "%s %s", netcodes[i].code, netcodes[i].label);
      }
      wrefresh(win);
      return (Menu_state) GRID;
    case '+':
      return (Menu_state) ADD;
    }
  }
}

Menu_state grid_movement(Position *pos) {
  int choice;
  while (1) {
    choice = getch();
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
      if (pos->curr_row < (int)pos->height) {
        pos= move_cur(pos, pos->curr_row + 1,
                            pos->curr_col);
      }
      break;
    case KEY_RIGHT:
      if (pos->curr_col < (int)pos->width) {
        pos= move_cur(pos, pos->curr_row,
                            pos->curr_col + 1);
      }
      break;
    case '\n':
      move_cur(pos, pos->curr_row, pos->curr_col);
      return (Menu_state) SELECT;
      break;
    };

    if (choice == 'q') {
      return (Menu_state) EXIT;
      break;
    }
  }
}

Proximal_states get_proximal_states(Position *pos, Grid_state *grid_state) {
  Grid_state left;
  Grid_state right;
  Grid_state up;
  Grid_state down;

  if (pos->curr_col > 1) {
    left = grid_state[(pos->curr_row - 1) * pos->width + (pos->curr_col - 2)];
  } else {
    left = (Grid_state){E, false};
  }

  if (pos->curr_col < (int)pos->width) {
    right = grid_state[(pos->curr_row - 1) * pos->width + (pos->curr_col)];
  } else {
    right = (Grid_state){E, false};
  }

  if (pos->curr_row > 1) {
    up = grid_state[(pos->curr_row - 2) * pos->width + (pos->curr_col - 1)];
  } else {
    up = (Grid_state){E, false};
  }

  if (pos->curr_row < (int)pos->height) {
    down = grid_state[(pos->curr_row) * pos->width + (pos->curr_col - 1)];
  } else {
    down = (Grid_state){E, false};
  }

  return (Proximal_states) {
    left, right, up, down};
}

void create_wall(Position *pos, Proximal_states p_states) {
    for (int col = 1; col < GRID_WIDTH - 1; col++) {
      if (p_states.up.is_wall == true) {
        mvwaddstr(get_pos(pos, pos->curr_row,
                          pos->curr_col), 0, col, "\u2588");
      } else {
        mvwaddstr(get_pos(pos, pos->curr_row,
                          pos->curr_col), 0, col, "\u2584");
      }

      if (p_states.down.is_wall == true) {
        mvwaddstr(get_pos(pos, pos->curr_row,
                          pos->curr_col), 2, col, "\u2588");
      } else {
        mvwaddstr(get_pos(pos, pos->curr_row,
                          pos->curr_col), 2, col, "\u2580");
      }
    }

    if (p_states.left.is_wall == true) {
      mvwaddstr(get_pos(pos, pos->curr_row,
                        pos->curr_col), 1, 0, "\u2588");
    } else {
      mvwaddstr(get_pos(pos, pos->curr_row,
                        pos->curr_col), 1, 0, "\u2590");
    }

    if (p_states.right.is_wall == true) {
      mvwaddstr(get_pos(pos, pos->curr_row,
                        pos->curr_col), 1, 4, "\u2588");
    } else {
      mvwaddstr(get_pos(pos, pos->curr_row,
                        pos->curr_col), 1, 4, "\u258C");
    }

    for (int col = 1; col < GRID_WIDTH - 1; col++) {
      mvwaddstr(get_pos(pos, pos->curr_row,
                        pos->curr_col), 1, col, "\u2588");
    }


    move_cur(pos, pos->curr_row, pos->curr_col);
    wrefresh(get_pos(pos, pos->curr_row, pos->curr_col));
  
}

Menu_state create_wall_mode(Position *pos, Grid_state *grid_state) {
  int choice = '\n';
  while (1) {
  Proximal_states p_states = get_proximal_states(pos, grid_state);

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
      if (pos->curr_row < (int)pos->height) {
        pos= move_cur(pos, pos->curr_row + 1,
                            pos->curr_col);
      }
      break;
    case KEY_RIGHT:
      if (pos->curr_col < (int)pos->width) {
        pos= move_cur(pos, pos->curr_row,
                            pos->curr_col + 1);
      }
      break;
    case '\n':
      /* move_cur(pos, pos->curr_row, pos->curr_col); */
      /* return (Menu_state) SELECT; */
      grid_state[(pos->curr_row - 1) * pos->width + (pos->curr_col - 1)].is_wall = true;
      create_wall(pos, p_states);
      break;
    case 'q':
      return (Menu_state) SELECT;
    };
    choice = getch();
  }
  return (Menu_state) GRID;
}


int main() {
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  start_color();
  init_pair(1, COLOR_MAGENTA, COLOR_MAGENTA);
  size_t width = 30;
  size_t height = 20;
  Grid_state *grid_state=
    (Grid_state *)malloc(sizeof(Grid_state) * height * width);


  Position *position =
    (Position *)malloc((sizeof(int) * 2) + (sizeof(size_t) * 2) +
                         (sizeof(WINDOW*) * height * width));
  position->height = height;
  position->width = width;
  position->curr_col = 1;
  position->curr_row = 1;

  Point center = center_grid(height, width);

  size_t first_row = center.y;
  size_t first_col = center.x;
  size_t last_row = (height - 1) * 2 + first_row;
  size_t last_col = (width - 1) * 4 + first_col;

  WINDOW *menu = newwin(getmaxy(stdscr), 20, 0, 0);
  refresh();
  box(menu, 0, 0);
  wrefresh(menu);
  center_horizontal(menu, "NET CODES", 2);
  center_horizontal(menu, "-----------", 3);

  Netcode netcodes[21] = {
    {"\u2588", "Wall"},
    {"=", "Gate 2"},
    {"\u2261", "Gate 3"},
    {"\u2263", "Gate 4"}};

  size_t net_menu_len = 4;

  for (size_t i = 0; i < net_menu_len; i++) {
    mvwprintw(menu, i + 5, 2, "%s %s", netcodes[i].code, netcodes[i].label);
  }

  wrefresh(menu);

  refresh();

  size_t i = 0;
  for (size_t row = first_row; row <= last_row; row += 2) {
    for (size_t col = first_col; col <= last_col; col += 4) {
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
        grid_state[i] = (Grid_state){BL, false};
      }
      position->grid[i] = win;
      i++;
    }
  }

  position = move_cur(position, 1, 1);
  keypad(stdscr, true);
  Menu_state menu_state = GRID;
  while (1) {
    switch (menu_state) {
    case GRID:
      menu_state = grid_movement(position);
      break;
    case ADD:
      break;
    case SELECT:
      curs_set(0);
      menu_state = net_menu(position, menu, netcodes, net_menu_len);
      curs_set(1);
      move_cur(position, position->curr_row, position->curr_col);
      break;
    case EXIT:
      free(position);
      endwin();
      return 0;
    case WALL:
      menu_state = create_wall_mode(position, grid_state);
      break;
    }
  }

  free(position);
  endwin();

  return 0;
}


  /* for (int col = 1; col < GRID_WIDTH - 1; col++) { */
  /*   if (p_states.up.is_wall == true) { */
  /*     mvwaddstr(get_pos(pos, pos->curr_row, */
  /*                       pos->curr_col), 0, col, "\u2588"); */
  /*   } else { */
  /*     mvwaddstr(get_pos(pos, pos->curr_row, */
  /*                       pos->curr_col), 0, col, "\u2584"); */
  /*   } */

  /*   if (p_states.down.is_wall == true) { */
  /*     mvwaddstr(get_pos(pos, pos->curr_row, */
  /*                       pos->curr_col), 2, col, "\u2588"); */
  /*   } else { */
  /*     mvwaddstr(get_pos(pos, pos->curr_row, */
  /*                       pos->curr_col), 2, col, "\u2580"); */
  /*   } */
  /* } */

  /* if (p_states.left.is_wall == true) { */
  /*   mvwaddstr(get_pos(pos, pos->curr_row, */
  /*                     pos->curr_col), 1, 0, "\u2588"); */
  /* } else { */
  /*   mvwaddstr(get_pos(pos, pos->curr_row, */
  /*                     pos->curr_col), 1, 0, "\u2590"); */
  /* } */

  /* if (p_states.right.is_wall == true) { */
  /*   mvwaddstr(get_pos(pos, pos->curr_row, */
  /*                     pos->curr_col), 1, 4, "\u2588"); */
  /* } else { */
  /*   mvwaddstr(get_pos(pos, pos->curr_row, */
  /*                     pos->curr_col), 1, 4, "\u258C"); */
  /* } */

  /* for (int col = 1; col < GRID_WIDTH - 1; col++) { */
  /*   mvwaddstr(get_pos(pos, pos->curr_row, */
  /*                     pos->curr_col), 1, col, "\u2588"); */
  /* } */


  /* move_cur(pos, pos->curr_row, pos->curr_col); */
  /* wrefresh(get_pos(pos, pos->curr_row, pos->curr_col)); */
