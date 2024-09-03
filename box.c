#include<curses.h>

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
