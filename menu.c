#include "netrunner.h"
#include <curses.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_code_label(WINDOW* win, Point point);
Netcode_array *initialize_netcodes();
void rebuild_symbols(Grid_state *grid, Position *pos);

void rebuild_symbols(Grid_state *grid, Position *pos) {
  int i = 0;
  for (unsigned row = 0; row < pos->height; row++) {
    for (unsigned col = 0; col < pos->width; col++) {
      if (grid[i].is_symbol) {
        WINDOW* win = get_pos(pos, row, col);
        print_char(win, 1, 2, grid[i].code);
      }
      i++;
    }
  }
}

int main_loop(Menu_state menu_state, Position *pos,
              Grid_state *grid_state, WINDOW* menu){

  Netcode_array *netcodes = initialize_netcodes();
  create_menu(menu, netcodes);
  move_cur(pos, 1, 1);
  FILE* in;

  while (1) {
    switch (menu_state) {
    case GRID:
      menu_state = grid_movement(pos);
      break;
    case ADD:
      netcodes = add_menu(menu, netcodes);
      wclear(menu);
      wrefresh(menu);
      create_menu(menu, netcodes);
      menu_state = SELECT;
      break;
    case SELECT:
      curs_set(0);
      menu_state = net_menu(grid_state, pos, menu, netcodes, netcodes->size);
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
    case REMOVE:
      clear_symbol(grid_state, pos);
      move_cur(pos, pos->curr_row, pos->curr_col);
      menu_state = GRID;
      break;
    case SAVE:
      mvprintw(2, 25, "Saving");
      menu_state = save_grid(grid_state, pos, netcodes);
      break;
    case LOAD:
      in = fopen("test.grid", "rb");
      pos = load_position(in);
      grid_state = load_grid_state(in, pos);
      netcodes = load_netcodes(in);
      build_ui(pos->height, pos->width, pos, grid_state);
      /* rebuild_symbols(grid_state, pos); */
      wmove(pos->grid[0], 1, 2);
      menu_state = GRID;
      break;
    }
  }
  return 0;
}

Menu_state net_menu(Grid_state *grid_state, Position *pos, WINDOW *win, Netcode_array *netcodes, size_t menu_len) {
  unsigned highlight = 0;
  int choice;
  while (1) {
    for (size_t i = 0; i < netcodes->size; i++) {
      if (i == highlight) {
        wattron(win, A_REVERSE);
        mvwprintw(win, i + 5, 2, "%s %s", netcodes->array[i].code, netcodes->array[i].label);
        wattroff(win, A_REVERSE);
      } else {
        mvwprintw(win, i + 5, 2, "%s %s", netcodes->array[i].code, netcodes->array[i].label);
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
        mvwprintw(win, i + 5, 2, "%s %s", netcodes->array[i].code, netcodes->array[i].label);
      }
      wrefresh(win);
      if (highlight == 0) {
        return (Menu_state) WALL;
      } else {
        print_center_char(pos, netcodes->array[highlight].code,
                   pos->curr_row, pos->curr_col);
        update_grid_code(grid_state, pos, pos->curr_row, pos->curr_col,
                         netcodes->array[highlight].code, true);
        return (Menu_state) GRID;
      }
      break;
    case 'q':
      for (size_t i = 0; i < menu_len ; i++) {
        mvwprintw(win, i + 5, 2, "%s %s", netcodes->array[i].code, netcodes->array[i].label);
      }
      wrefresh(win);
      return (Menu_state) GRID;
    case '+':
      return (Menu_state) ADD;
    case 's':
      return (Menu_state) SAVE;
    case 'l':
      return (Menu_state) LOAD;
    }
  }
}

Netcode_array *add_menu(WINDOW *win, Netcode_array *netcodes) {
  char codes[][10] = {"\u2558", "=", "\u2261", "\u2261"};
  size_t codes_len = sizeof(codes) / sizeof(char[10]);

  wclear(win);
  keypad(win, true);
  box(win, 0, 0);
  curs_set(0);
  center_horizontal(win, "SELECT CODE", 2);
  center_horizontal(win, "-----------", 3);

  unsigned highlight = 0;
  int choice;
  int loop = true;

  while (loop) {
    for (unsigned i = 0; i < codes_len; i++) {
      if (i == highlight) {
        wattron(win, A_REVERSE);
        mvwprintw(win, i + 5, 2, "%s", codes[i]);
        wattroff(win, A_REVERSE);
      } else {
        mvwprintw(win, i + 5, 2, "%s", codes[i]);
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
      if (highlight < codes_len) {
        highlight++;
      }
      break;
    case '\n':
      loop = false;
      char new_code[10];
      strcpy(new_code, codes[highlight]);
      char new_label[30];
      strcpy(new_label, get_code_label(win, (Point){highlight+5, 4}));
      add_code(netcodes, new_code, new_label);
      create_menu(win, netcodes);
      break;
    }
  }

  return netcodes;
}

void create_menu(WINDOW *win, Netcode_array *netcodes) {
  refresh();
  box(win, 0, 0);
  wrefresh(win);
  center_horizontal(win, "NET CODES", 2);
  center_horizontal(win, "-----------", 3);

  for (unsigned i = 0; i < netcodes->size; i++) {
    mvwprintw(win, i + 5, 2, "%s %s", netcodes->array[i].code, netcodes->array[i].label);
  }
  wrefresh(win);
}

Netcode_array *initialize_netcodes() {
  Netcode codes[] = {
    {"\u2588", "Wall Mode"},
    {"\u2261", "Gate 222"},
    {"\u2261", "Gate 3asdf"},
    {"\u2263", "Gate 4"}};
  size_t codes_len = sizeof(codes) / sizeof(Netcode);

  Netcode_array *netcodes;
  if ((netcodes = (Netcode_array*)malloc(sizeof(Netcode_array))) == NULL)
    err(1, "Netcodes initialization out of memoery");

  if ((netcodes->array = calloc(codes_len, sizeof(Netcode)))
      ==NULL)
    err(1, "Netcodes array initailization out of memory");

  netcodes->size = codes_len;

  for (size_t i = 0; i < codes_len; i++) {
    netcodes->array[i] = codes[i];
  }
  return netcodes;
}

Netcode_array *add_code(Netcode_array *netcodes, char* new_code, char* new_label) {

  netcodes->array = reallocarray(netcodes->array, netcodes->size+1, sizeof(Netcode));

  netcodes->size += 1;
  strcpy(netcodes->array[netcodes->size - 1].code, new_code);
  strcpy(netcodes->array[netcodes->size - 1].label, new_label);

  return netcodes;
}

char* get_code_label(WINDOW* win, Point point) {
  char *str = malloc(sizeof(char[30]));
  curs_set(1);
  echo();
  mvwgetnstr(win, point.y, point.x, str, 29);
  noecho();
  if (sizeof(*str) > 29) {
    fprintf(stderr, "String too long");
    exit(1);
  }
  return str;
}
