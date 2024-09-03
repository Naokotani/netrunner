#include <stdbool.h>

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
  Grid_state center;
}Proximal_states;

typedef struct {
  char *code;
  char *label;
} Netcode;

typedef struct {
  unsigned curr_row;
  unsigned curr_col;
  unsigned width;
  unsigned height;
  WINDOW *grid[];
} Position;
