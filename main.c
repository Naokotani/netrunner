#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

typedef struct {
  int y;
  int x;
} Point;

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
};

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

int main() {
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  size_t width = 20;
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

  print_char(position, "\u20ac", 4, 4);
  print_char(position, "\u0040", 6, 8);
  print_char(position, "\u00A5", 3, 11);

  int choice;
  position = move_cur(position, 1, 1);
  keypad(stdscr, true);
  while (1) {
    choice = getch();
    switch (choice) {
    case KEY_UP:
      if (position->curr_row > 1) {
        position = move_cur(position, position->curr_row - 1,
                            position->curr_col);
      }
      break;
    case KEY_LEFT:
      if (position->curr_col > 1) {
        position = move_cur(position, position->curr_row,
                            position->curr_col - 1);
      }
      break;
    case KEY_DOWN:
      if (position->curr_row < (int)position->height) {
        position = move_cur(position, position->curr_row + 1,
                            position->curr_col);
      }
      break;
    case KEY_RIGHT:
      if (position->curr_col < (int)position->width) {
        position = move_cur(position, position->curr_row,
                            position->curr_col + 1);
      }
      break;
        };

        if (choice == 'q') {
          break;
        }
  }

  free(position);
  endwin();


  return 0;
}
