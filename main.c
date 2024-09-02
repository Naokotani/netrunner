#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

typedef struct {
  int y;
  int x;
} Point;

typedef enum  {
  ADD,
  GRID,
  SELECT,
  EXIT
} Menu_state;

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
      print_char(pos, netcodes[highlight].code,
               pos->curr_row, pos->curr_col);
      return (Menu_state) GRID;
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

int main() {
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  start_color();
  size_t width = 30;
  size_t height = 20;
  Position *position =
    (Position *)malloc((sizeof(int) * 2) + (sizeof(size_t) * 2) +
                         (sizeof(WINDOW*) * height * width + 1));
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
    {"\u20ac", "foo"},
    {"\u0040", "bar"},
    {"\u00a5", "baz"}};

  size_t net_menu_len = 3;

  for (size_t i = 0; i < net_menu_len; i++) {
    mvwprintw(menu, i + 5, 2, "%s %s", netcodes[i].code, netcodes[i].label);
  }

  wrefresh(menu);
  curs_set(2);

  size_t i = 0;
  for (size_t row = first_row; row <= last_row; row += 2) {
    for (size_t col = first_col; col <= last_col; col += 4) {
      refresh();
      WINDOW *win = newwin(3, 5, row, col);
      if (row == first_row && col == first_col) {
        top_left_box(win);
      } else if (row == first_row && col != last_col) {
        top_box(win);
      } else if (row == first_row && col == last_col) {
        top_right_box(win);
      } else if (row != first_row && col == first_col && row != last_row) {
        left_box(win);
      } else if (row != first_row && col != first_col &&
                 row != last_row && col != last_col) {
        mid_box(win);
      } else if (row != first_row && row != last_row && col == last_col) {
        right_box(win);
      } else if (row == last_row && col == first_col) {
        bottom_left_box(win);
      } else if (row == last_row && col != last_col) {
        bottom_box(win);
      } else if (row == last_row && col == last_col) {
        bottom_right_box(win);
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
    }
  }

  free(position);
  endwin();

  return 0;
}
