#include "netrunner.h"
#include <stdio.h>
#include <string.h>

void clear_symbol(Grid_state *grid_state, Position *pos) {
  print_center_char(pos, " ", pos->curr_row, pos->curr_col);
  update_grid_code(grid_state, pos, pos->curr_row,
                   pos->curr_col, "3", false);
}

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

WINDOW *get_pos(Position *pos, size_t row, size_t col) {
  WINDOW *win = pos->grid[(row - 1) * pos->width + (col - 1)];
  return win;
}

WINDOW *print_center_char(Position *pos, char* code, size_t row, size_t col) {
  WINDOW *win = get_pos(pos, row, col);
  mvwprintw(win, 1, 2, "%s", code);
  wrefresh(win);
  return win;
}

void update_grid_code(Grid_state *grid_state, Position *pos,
                      unsigned row, unsigned col, char* code, bool status) {
  strcpy(grid_state[(row - 1 ) * pos->width + (col - 1)].code, code);
  grid_state[(row - 1 ) * pos->width + (col - 1)].is_symbol = status;
}

void print_char(WINDOW *win, unsigned row, unsigned col, char* code) {
  mvwprintw(win, row, col, "%s", code);
  wrefresh(win);
}

Position *move_cur(Position *pos, size_t row, size_t col) {
  WINDOW *win = get_pos(pos, row, col);
  wmove(win, 1, 2);
  wrefresh(win);
  pos->curr_col = col;
  pos->curr_row = row;
  return pos;
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
      move_cur(pos, pos->curr_row, pos->curr_col);
      return (Menu_state)SELECT;
    case 's':
      return (Menu_state) SAVE;
    case 'l':
      return (Menu_state) LOAD;
    case 'q':
      return (Menu_state) EXIT;
    case 'r':
      return (Menu_state) REMOVE;
    };
  }
}
