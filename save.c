#include "netrunner.h"
#include <curses.h>
#include <stdio.h>

Menu_state save_grid(Grid_state *grid_state, Position *pos, Netcode_array *netcodes) {
  FILE* out;
  out = fopen("test.grid","wb");
  
  if (out == NULL) {
    err(1, "Save file failed to open");
  }

  size_t size = pos->height * pos->width;
  Save_position save_position = {
    pos->curr_row, pos->curr_col, pos->width, pos->height
  };

  size_t netcode_size = netcodes->size;

  fwrite(&save_position, sizeof(Save_position), 1, out);
  fwrite(grid_state, sizeof(Grid_state), size, out);
  fwrite(&netcode_size, sizeof(size_t), 1, out);
  fwrite(netcodes->array, sizeof(Netcode), netcode_size, out);

  fclose(out);

  return (Menu_state)GRID;
};

Netcode_array *load_netcodes(FILE *file) {

  size_t size;
  size_t res = fread(&size, sizeof(size_t), 1, file);

  if (res != 1) {
    err(1, "Failed to read netcode size");
  }

  Netcode *codes;
  codes = allocate_array(size, sizeof(Netcode));

  size_t codes_read = fread(codes, sizeof(Netcode), size, file);

  if (codes_read != size) {
    err(1, "Failed to read net codes.");
  }

  Netcode_array *netcodes = allocate_struct(sizeof(Netcode_array));
  netcodes->array = allocate_array(size, sizeof(Netcode));

  netcodes->array = codes;
  netcodes->size = size;

  free(codes);

  return netcodes;
};

Position *load_position(FILE *file) {
  Save_position save_position;
  fread(&save_position, sizeof(Save_position), 1, file);

  Position *pos = allocate_struct(sizeof(Position));
  *pos->grid = allocate_array(save_position.height * save_position.width,
                                 sizeof(WINDOW*));

  pos->curr_row = save_position.curr_row;
  pos->curr_col = save_position.curr_col;
  pos->height = save_position.height;
  pos->width = save_position.width;

  return pos;
}

Grid_state *load_grid_state(FILE *file, Position *pos) {
  size_t size = pos->height * pos->width;
  Grid_state *grid_state = allocate_array(size, sizeof(Grid_state));
  size_t res = fread(grid_state, sizeof(Grid_state), size, file);

  if (res != size)
    err(1, "Error reading grid state array");

  return grid_state;
}


