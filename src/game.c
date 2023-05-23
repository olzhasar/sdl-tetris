#include "game.h"
#include "definitions.h"
#include "graphics.h"
#include "input.h"

static const int REGULAR_FREQ = 32;
static const int SOFT_FREQ = 4;
static const int HARD_FREQ = 1;

static int game_over = 0;
static int score = 0;

static int drop_freq = REGULAR_FREQ;
static int iteration = 0;

// Grid is represented as m x n int matrix. Values are color codes for occupied
// cells or 0 for empty cells
static int grid[GRID_WIDTH][GRID_HEIGHT] = {0};
// Array of rows that need to be destroyed
static int to_destroy[GRID_HEIGHT] = {0};

static const int N_COLORS = 13;
static const int COLORS[13] = {
    0xFFC82E, 0xFEFB34, 0x53DA3F, 0x01EDFA, 0xDD0AB2, 0xEA141C, 0xFE4819,
    0xFF910C, 0x39892F, 0x0077D3, 0x78256F, 0x2E2E84, 0x485DC5,
};

// Array of blocks in the current shape
// Each value pair up to index 8 corresponds to the shift from the shape
// position over x and y axis
// Last value is the color of the current shape
static int current_shape[9] = {0};
static int current_shape_type;

// Current shape coordinates
static int current_x = 0, current_y = 0;

// Represent shapes as an array of 8 ints.
// Each int pair represents the shift from the shape position over x and y axis
static const int N_SHAPES = 7;
static int SHAPES[7][8] = {
    {0, 0, 1, 0, 0, 1, 1, 1},   // O
    {0, 0, -1, 0, 1, 0, 0, 1},  // T
    {0, 0, 0, -1, 0, 1, 1, 1},  // L
    {0, 0, 0, -1, 0, 1, -1, 1}, // J
    {0, 0, 0, -1, 0, 1, 0, 2},  // I
    {0, 0, 1, 0, 0, 1, -1, 1},  // S
    {0, 0, -1, 0, 0, 1, 1, 1},  // Z
};

static const int FRAME_DELAY = 16; // 1000 / 16 ~= 60fps
static const int RESTART_DELAY = 300;

static const int SCORE_SINGLE = 1;
static const int SCORE_LINE = 100;

void restart_game() {
  for (int i = 0; i < GRID_WIDTH; i++) {
    for (int j = 0; j < GRID_HEIGHT; j++) {
      grid[i][j] = 0;
    }
  };

  game_over = 0;
  score = 0;

  SDL_Delay(RESTART_DELAY);
}

void end_game() {
  game_over = 1;
  clear_screen();
}

void spawn_shape() {
  current_shape_type = rand() % N_SHAPES;

  for (int i = 0; i < 8; i++) {
    current_shape[i] = SHAPES[current_shape_type][i];
  }

  current_shape[8] = COLORS[rand() % N_COLORS];

  current_x = GRID_WIDTH / 2;
  current_y = 0;
}

void destroy_row(int row) {
  for (int j = row; j > 0; j--) {
    for (int i = 0; i < GRID_WIDTH; i++) {
      grid[i][j] = grid[i][j - 1];
    }
  }
}

void clean_destroyed_blocks() {
  int count = 0;

  for (int j = 0; j < GRID_HEIGHT; j++) {
    if (to_destroy[j]) {
      count++;

      to_destroy[j] = 0;
      for (int i = 0; i < GRID_WIDTH; i++) {
        grid[i][j] = 0;
      }
      destroy_row(j);
    }
  }

  if (count) {
    score += SCORE_LINE * (1 + 2 * (count - 1));
  }
}

int row_is_full(int y) {
  if (y < 0) { // can be negative at the end of the game
    return 1;
  }

  for (int i = 0; i < GRID_WIDTH; i++) {
    if (grid[i][y] == 0) {
      return 0;
    }
  }

  to_destroy[y] = 1;
  return 1;
}

void lock_shape() {
  int i;
  int x, y;

  for (i = 0; i < 4; i++) {
    x = current_shape[i * 2] + current_x;
    y = current_shape[i * 2 + 1] + current_y;

    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
      grid[x][y] = current_shape[8];
    }

    if (!row_is_full(y)) {
      if (y <= 0) {
        end_game();
      }
    }
  }

  iteration = 0;
  score += SCORE_SINGLE;
  drop_freq = REGULAR_FREQ;
}

int detect_collision(int x, int y) {
  if (x < 0 || x >= GRID_WIDTH) {
    return 1;
  }

  if (y >= GRID_HEIGHT) { // collisions at the top are OK
    return 1;
  }

  if (y >= 0 && grid[x][y]) {
    return 1;
  }

  return 0;
}

void rotate_shape() {
  drop_freq = REGULAR_FREQ;

  if (current_shape_type == 0) {
    return; // O-shape should not be rotated
  }

  int temp[8] = {0};

  int x, y;

  for (int i = 0; i < 4; i++) {
    temp[i * 2] = current_shape[i * 2 + 1];
    temp[i * 2 + 1] = -current_shape[i * 2];

    x = temp[i * 2] + current_x;
    y = temp[i * 2 + 1] + current_y;

    if (detect_collision(x, y)) {
      return;
    }
  };

  for (int i = 0; i < 8; i++) {
    current_shape[i] = temp[i];
  }
}

void move_side(int n) {
  drop_freq = REGULAR_FREQ; // Stop fast dropping fast
  int x, y;

  for (int i = 0; i < 4; i++) {
    x = current_shape[i * 2] + current_x + n;
    y = current_shape[i * 2 + 1] + current_y;

    if (detect_collision(x, y)) {
      return;
    }
  }

  current_x += n;
}

void move_down() {
  // Move only once in `drop_freq` times
  if (iteration < drop_freq) {
    return;
  }

  iteration = 0;

  int x, y;

  for (int i = 0; i < 4; i++) {
    x = current_shape[i * 2] + current_x;
    y = current_shape[i * 2 + 1] + current_y + 1;

    if (detect_collision(x, y)) {
      lock_shape();
      spawn_shape();
      return;
    }
  }

  current_y += 1;
}

void handle_input_event(enum InputEvent event) {
  switch (event) {
  case LEFT:
    return move_side(-1);
  case RIGHT:
    return move_side(1);
  case ROTATE:
    return rotate_shape();
  case HARD_DROP:
    drop_freq = HARD_FREQ;
    break;
  case SOFT_DROP:
    drop_freq = SOFT_FREQ;
    break;
  default:
    break;
  }
  move_down();
}

void update_frame() {
  if (game_over) {
    render_game_over_message(score);
    return;
  }

  clear_screen();

  for (int i = 0; i < GRID_WIDTH; ++i) {
    for (int j = 0; j < GRID_HEIGHT; ++j) {
      draw_block(i, j, grid[i][j]);
    }
  }

  int x, y;

  for (int i = 0; i < 4; i++) {
    x = current_shape[i * 2] + current_x;
    y = current_shape[i * 2 + 1] + current_y;

    if (y >= 0) { // skip overflowed
      draw_block(x, y, current_shape[8]);
    }
  }

  render_frame(score);
}

int init_game() {
  spawn_shape();

  return init_graphics();
}

int game_loop() {
  enum InputEvent event = listen_for_input(game_over);
  if (event == QUIT) {
    return 1;
  }

  if (game_over) {
    if (event > 0) {
      restart_game();
    }
  } else {
    handle_input_event(event);
    clean_destroyed_blocks();
    update_frame();
    iteration++;

    SDL_Delay(FRAME_DELAY);
  }

  return 0;
}

int terminate_game() {
  release_resources();
  return 0;
}
