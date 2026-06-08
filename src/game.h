#pragma once
#define GRID_WIDTH 10
#define GRID_HEIGHT 20
#define N_SHAPES 7

typedef enum {
  QUIT = -1,
  NO_INPUT,
  ANY_INPUT,
  LEFT,
  RIGHT,
  DOWN,
  ROTATE,
  SOFT_DROP,
  HARD_DROP,
  START,
} input_event_t;

typedef struct {
  unsigned int score;
  unsigned int level;
  // Grid is represented as m x n int matrix. Values are color indices for
  // occupied cells or 0 for empty cells
  int grid[GRID_WIDTH][GRID_HEIGHT];
  // Array of blocks in the current shape
  // Each value pair corresponds to the shift from the shape
  // position over x and y axis
  int current_shape[8];
  // Index of the color in the COLORS array
  int current_shape_kind;
  // Current shape coordinates
  int current_x;
  int current_y;

  unsigned int game_over;
  unsigned int changed;
} game_state_t;

game_state_t game_state_new();
void game_state_update(game_state_t *state, input_event_t event);
