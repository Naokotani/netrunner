#include "netrunner.h"
#include <curses.h>
#define CODES_LEN 3

void create_menu(WINDOW *win, Netcode_array *netcodes) {
  refresh();
  box(win, 0, 0);
  wrefresh(win);
  center_horizontal(win, "NET CODES", 2);
  center_horizontal(win, "-----------", 3);

  for (unsigned i = 0; i < netcodes->size; i++) {
    mvwprintw(win, i + 5, 2, "%s %s", netcodes->array[i]->code, netcodes->array[i]->label);
  }

  wrefresh(win);
}

void add_menu(WINDOW *win, Netcode *netcodes) {
  char *codes[CODES_LEN] = {"\u2558", "=", "\u2261"};

  wclear(win);
  box(win, 0, 0);
  center_horizontal(win, "SELECT CODE", 2);
  center_horizontal(win, "-----------", 3);
  for (unsigned i = 0; i < CODES_LEN; i++) {
    mvwprintw(win, i + 5, 2, "%s", codes[i]);
  }
  wrefresh(win);
}

Menu_state net_menu(Position *pos, WINDOW *win, Netcode *netcodes[], size_t menu_len) {
  unsigned highlight = 0;
  int choice;
  while (1) {
    for (size_t i = 0; i < menu_len; i++) {
      if (i == highlight) {
        wattron(win, A_REVERSE);
        mvwprintw(win, i + 5, 2, "%s %s", netcodes[i]->code, netcodes[i]->label);
        wattroff(win, A_REVERSE);
      } else {
        mvwprintw(win, i + 5, 2, "%s %s", netcodes[i]->code, netcodes[i]->label);
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
        mvwprintw(win, i + 5, 2, "%s %s", netcodes[i]->code, netcodes[i]->label);
      }
      wrefresh(win);
      if (highlight == 0) {
        return (Menu_state) WALL;
      } else {
        print_center_char(pos, netcodes[highlight]->code,
                   pos->curr_row, pos->curr_col);
        return (Menu_state) GRID;
      }
      break;
    case 'q':
      for (size_t i = 0; i < menu_len ; i++) {
        mvwprintw(win, i + 5, 2, "%s %s", netcodes[i]->code, netcodes[i]->label);
      }
      wrefresh(win);
      return (Menu_state) GRID;
    case '+':
      add_menu(win, *netcodes);
      return (Menu_state) ADD;
    }
  }
}
