#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
  int y;
  int x;
} Center;

typedef struct {
  int y;
  int x;
} Point;

typedef struct {
  Point tl;
  Point tr;
  Point br;
  Point bl;
  Point top;
  Point bottom;
  Point right;
  Point left;
  size_t max_x_col;
  size_t max_y_row;
} Coordinates;

typedef struct {
  WINDOW *tl;
  WINDOW *tr;
  WINDOW *bl;
  WINDOW *br;
} Corners;

typedef struct {
  size_t size;
  WINDOW *windows[];
} Win_array;

Center center_content(int content_width) {
  Center c;
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
  for (size_t i = 0; i < size; i++) {
    wborder(win[i], ACS_VLINE, ' ',
            ACS_HLINE, ' ', ACS_TTEE, ' ',
            ' ', ' ');
    wrefresh(win[i]);
  }
}

void mid_box(WINDOW *win[], size_t size) {
  for (size_t i = 0; i < size; i++) {
    wborder(win[i], ACS_VLINE, ' ',
            ACS_HLINE, ' ', ACS_PLUS, ' ',
            ' ' , ' ');
    wrefresh(win[i]);
  }
}

void left_box(WINDOW *win[], size_t size) {
  for (size_t i = 0; i < size; i++) {
    wborder(win[i], ACS_VLINE, ' ',
            ACS_HLINE, ' ', ACS_LTEE, ' ',
            ' ', ' ');
    wrefresh(win[i]);
  }
}

void right_box(WINDOW *win[], size_t size) {
  for (size_t i = 0; i < size; i++) {
    wborder(win[i], ACS_VLINE, ACS_VLINE,
            ACS_HLINE, ' ', ACS_PLUS, ACS_RTEE,
            ' ', ' ');
    wrefresh(win[i]);
  }
}

void bottom_box(WINDOW *win[], size_t size) {
  for (size_t i = 0; i < size; i++) {
    wborder(win[i], ACS_VLINE, ' ',
            ACS_HLINE, ACS_HLINE, ACS_PLUS, ' ',
            ACS_BTEE, ' ');
    wrefresh(win[i]);
  }
}

void *create_box_array(size_t size, int multiplier) {
  return malloc((size - 2) * sizeof(WINDOW*) * multiplier);
}


Win_array *generate_alt_top_windows(Coordinates c, Win_array *win_array) {
  size_t i = 0;

  for (size_t col = 5; col < c.max_x_col; col += 4) {
    win_array->windows[i] = newwin(3, 5, 1, col);
    i++;
  };

  win_array->size = i + 1;
  refresh();
  top_box(win_array->windows, win_array->size);

  return win_array;
}

Win_array *generate_alt_left_windows(Coordinates c, Win_array *win_array) {
  size_t i = 0;
  for (size_t row = 3; row < c.max_y_row; row += 2) {
    win_array->windows[i] = newwin(3, 5, row, 1);
    i++;
  };

  win_array->size = i + 1;
  refresh();
  left_box(win_array->windows, win_array->size);

  return win_array;
}


Win_array *generate_alt_right_windows(Coordinates c, Win_array *win_array) {
  size_t i = 0;
  for (size_t row = 3; row < c.max_y_row; row += 2) {
    win_array->windows[i] = newwin(3, 5, row, c.max_x_col);
    i++;
  };

  win_array->size = i + 1;
  refresh();
  right_box(win_array->windows, win_array->size);

  return win_array;
}

Win_array *generate_alt_bot_windows(Coordinates c, Win_array *win_array) {
  size_t i = 0;

  for (size_t col = 5; col < c.max_x_col; col += 4) {
    win_array->windows[i] = newwin(3, 5, c.max_y_row, col);
    i++;
  };

  win_array->size = i + 1;
  refresh();
  bottom_box(win_array->windows, win_array->size);
  return win_array;
}


Win_array *generate_alt_mid_windows(Coordinates c, Win_array *win_array) {
  size_t i = 0;

  for (size_t col = 5; col < c.max_x_col; col += 4) {
    for (size_t row = 3; row < c.max_y_row; row += 2) {
      win_array->windows[i] = newwin(3, 5, row, col);
      i++;
    }
  };

  win_array->size = i;
  mvprintw(1, 30, "%lu", win_array->size);
  refresh();
  mid_box(win_array->windows, win_array->size);

  return win_array;
}

Coordinates get_coordinates(size_t grid_size) {
  size_t max_x_col = (grid_size - 2) * 4 + 5;
  size_t max_y_row = (grid_size - 2) * 2 + 3;
  /* size_t mid_grid_size = (grid_size - 2) * (grid_size -2); */

  Coordinates coordinates = {
    {1, 1},
    {1, max_x_col},
    {max_y_row, max_x_col},
    {max_y_row, 1},
    {1, 5},
    {max_y_row, 5},
    {3, max_x_col},
    {3, 1},
    max_x_col,
    max_y_row
  };

  return coordinates;
}

int main() {
  initscr();
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_MAGENTA);
  char content[] = "This content is centered.";
  int content_length = strlen(content);
  Center c = center_content(content_length);
  move(c.y, c.x);
  addstr(content);

  size_t grid_size = 8;
  Coordinates coordinates = get_coordinates(grid_size);

  Corners corners = {
    newwin(3, 5, coordinates.tl.y, coordinates.tl.x),
    newwin(3, 5, coordinates.tr.y, coordinates.tr.x),
    newwin(3, 5, coordinates.bl.y, coordinates.bl.x),
    newwin(3, 5, coordinates.br.y, coordinates.br.x)
  };
  refresh();

 Win_array *top_boxes_alt =
   (Win_array *) malloc
   (sizeof(size_t) + sizeof(WINDOW *) * grid_size - 2);
 Win_array *bottom_boxes_alt =
   (Win_array *) malloc
   (sizeof(size_t) + sizeof(WINDOW *) * grid_size - 2);
 Win_array *mid_boxes_alt =
   (Win_array *) malloc
   (sizeof(size_t) + sizeof(WINDOW *) * (grid_size - 2) * (grid_size - 2));
 Win_array *left_boxes_alt =
   (Win_array *) malloc
   (sizeof(size_t) + sizeof(WINDOW *) * grid_size - 2);
 Win_array *right_boxes_alt =
   (Win_array *) malloc
   (sizeof(size_t) + sizeof(WINDOW *) * grid_size - 2);

 top_left_box(corners.tl);
 top_boxes_alt =
   generate_alt_top_windows(coordinates, top_boxes_alt);
 top_right_box(corners.tr);
 left_boxes_alt =
   generate_alt_left_windows(coordinates, left_boxes_alt);
 mid_boxes_alt =
   generate_alt_mid_windows(coordinates, mid_boxes_alt);
 right_boxes_alt =
   generate_alt_right_windows(coordinates, right_boxes_alt);
 bottom_left_box(corners.bl);
 bottom_boxes_alt =
   generate_alt_bot_windows(coordinates, bottom_boxes_alt);
 bottom_right_box(corners.br);
 refresh();

  getch();
  endwin();

  free(top_boxes_alt);
  free(bottom_boxes_alt);
  free(mid_boxes_alt);
  free(left_boxes_alt);
  free(right_boxes_alt);

  return 0;
}

/* WINDOW **generate_top_windows(size_t size, WINDOW *win_array[]) { */
/*   int col = 5; */
/*   for (size_t i = 0; i < size - 2; i++) { */
/*     win_array[i] = newwin(3, 5, 1, col); */
/*     col += 4; */
/*   } */
/*   return win_array; */
/* } */

/* WINDOW **generate_left_windows(size_t size, WINDOW *win_array[]) { */
/*   int row = 3; */
/*   for (size_t i = 0; i < size - 2; i++) { */
/*     win_array[i] = newwin(3, 5, row, 1); */
/*     row += 2; */
/*   } */
/*   return win_array; */
/* } */

/* WINDOW **generate_right_windows(size_t size, WINDOW *win_array[]) { */
/*   int row = 3; */
/*   for (size_t i = 0; i < size - 2; i++) { */
/*     win_array[i] = newwin(3, 5, row, 5 + (size - 2) * 4); */
/*     row += 2; */
/*   } */
/*   return win_array; */
/* } */

/* WINDOW **generate_mid_windows(size_t size, WINDOW *win_array[]) { */
/*   int col = 5; */
/*   int row = 3; */

/*   for (size_t rows = size - 2; rows > 0; rows--) { */
/*     for (size_t i = 0; i < size - 2; i++) { */
/*       win_array[i] = newwin(3, 5, row, col); */
/*       col += 4; */
/*     } */
/*     row += 2; */
/*     col = 5; */
/*   } */
/*   return win_array; */
/* } */

/* WINDOW **generate_bottom_windows(size_t size, WINDOW *win_array[]) { */
/*   int col = 5; */
/*   for (size_t i = 0; i < size - 2; i++) { */
/*     win_array[i] = newwin(3, 5, 3 + (size - 2) * 2, col); */
/*     col += 4; */
/*   } */
/*   return win_array; */
/* } */
