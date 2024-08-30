#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Center {
  int y;
  int x;
} center;

center center_content(int content_width) {
  struct Center c;
  getmaxyx(stdscr, c.y, c.x);
  c.x = c.x / 2 - content_width / 2;
  c.y = c.y / 2;
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
  wborder(win, ACS_VLINE, ACS_VLINE,
          ACS_HLINE, ACS_HLINE, ACS_PLUS, ACS_RTEE,
          ACS_BTEE, ACS_LRCORNER);
  wrefresh(win);
}

void top_box(WINDOW *win[], size_t size) {
  for (int i = 0; i < size; i++) {
    wborder(win[i], ACS_VLINE, ' ',
            ACS_HLINE, ' ', ACS_TTEE, ACS_HLINE,
            ' ', ' ');
    wrefresh(win[i]);
  }
}

void mid_box(WINDOW *win[], size_t size) {
  for (int i = 0; i < size; i++) {
    wborder(win[i], ACS_VLINE, ' ',
            ACS_HLINE, ' ', ACS_PLUS, ' ',
            ' ' , ' ');
    wrefresh(win[i]);
  }
}

void left_box(WINDOW *win[], size_t size) {
  for (int i = 0; i < size; i++) {
    wborder(win[i], ACS_VLINE, ' ',
            ACS_HLINE, ' ', ACS_LTEE, ' ',
            ' ', ' ');
    wrefresh(win[i]);
  }
}

void right_box(WINDOW *win[], size_t size) {
  for (int i = 0; i < size; i++) {
    wborder(win[i], ACS_VLINE, ACS_VLINE,
            ACS_HLINE, ' ', ACS_PLUS, ACS_RTEE,
            ' ', ' ');
    wrefresh(win[i]);
  }
}

void bottom_box(WINDOW *win[], size_t size) {
  for (int i = 0; i < size; i++) {
    wborder(win[i], ACS_VLINE, ' ',
            ACS_HLINE, ACS_HLINE, ACS_PLUS, ' ',
            ACS_BTEE, ' ');
    wrefresh(win[i]);
  }
}

void *create_box_array(size_t size, int multiplier) {
  return malloc((size - 2) * sizeof(WINDOW*) * multiplier);
}

WINDOW **generate_top_windows(size_t size, WINDOW *win_array[]) {
  int col = 5;
  for (int i = 0; i < size - 2; i++) {
    win_array[i] = newwin(3, 5, 1, col);
    col += 4;
  }
  return win_array;
}

WINDOW **generate_left_windows(size_t size, WINDOW *win_array[]) {
  int row = 3;
  for (int i = 0; i < size - 2; i++) {
    win_array[i] = newwin(3, 5, row, 1);
    row += 2;
  }
  return win_array;
}

WINDOW **generate_right_windows(size_t size, WINDOW *win_array[]) {
  int row = 3;
  for (int i = 0; i < size - 2; i++) {
    win_array[i] = newwin(3, 5, row, 5 + (size - 2) * 4);
    row += 2;
  }
  return win_array;
}

WINDOW **generate_bottom_windows(size_t size, WINDOW *win_array[]) {
  int col = 5;
  for (int i = 0; i < size - 2; i++) {
    win_array[i] = newwin(3, 5, 3 + (size - 2) * 2, col);
    col += 4;
  }
  return win_array;
}

WINDOW **generate_mid_windows(size_t size, WINDOW *win_array[]) {
  int col = 5;
  int row = 3;

  for (int i = 0; i < size - 2;) {
    win_array[i] = newwin(3, 5, row, col);
    col += 4;
    if (size - 2 == (row - 1) / 2) {
      col = 5;
      row += 2;
      i++;
    } 
  }
  return win_array;
}

int main() {
  initscr();
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_MAGENTA);
  char content[] = "This content is centered.";
  int content_length = strlen(content);
  struct Center c = center_content(content_length);
  move(c.y, c.x);
  addstr(content);

  size_t grid_size = 4;

  WINDOW** top_box_array = create_box_array(grid_size, 1);
  WINDOW** left_box_array = create_box_array(grid_size, 1);
  WINDOW** mid_box_array = create_box_array(grid_size, 2);
  WINDOW** right_box_array = create_box_array(grid_size, 1);
  WINDOW** bottom_box_array = create_box_array(grid_size, 1);

  /* WINDOW *tl = newwin(3, 5, 1, 1); */
  /* WINDOW *tr = newwin(3, 5, 1, 9); */
  /* WINDOW *bl = newwin(3, 5, 5, 1); */
  /* WINDOW *br = newwin(3, 5, 5, 9); */
  refresh();

  WINDOW **top_boxes = generate_top_windows(grid_size, create_box_array
                                            (grid_size, 1));
  WINDOW **mid_boxes = generate_mid_windows(grid_size, create_box_array
                                            (grid_size, 2));
  WINDOW **left_boxes = generate_left_windows(grid_size, create_box_array
                                            (grid_size, 1));
  WINDOW **right_boxes = generate_right_windows(grid_size, create_box_array
                                            (grid_size, 1));
  WINDOW **bottom_boxes =
    generate_bottom_windows(grid_size,
                            create_box_array(grid_size,
                                             1));

  /* top_left_box(tl); */
  top_box(top_boxes, grid_size - 2);
  /* top_right_box(tr); */
  left_box(left_boxes, grid_size - 2);
  mid_box(mid_boxes, grid_size - 2);
  right_box(right_boxes, grid_size - 2);
  /* bottom_left_box(bl); */
  bottom_box(bottom_boxes, grid_size - 2);
  /* bottom_right_box(br); */

  getch();
  endwin();

  free(top_box_array);
  free(left_box_array);
  free(mid_box_array);
  free(right_box_array);
  free(bottom_box_array);

  return 0;
}




/*
int wborder(WINDOW *win, chtype ls, chtype rs,
            chtype ts, chtype bs, chtype tl, chtype tr,
            chtype bl, chtype br);
  */


