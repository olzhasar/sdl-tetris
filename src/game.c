#include "game.h"

enum Action current_action = MOVE_DOWN;
int game_over = 0;
int score = 0;

int WAIT = 32;
int iteration = 0;

// Grid is represented as m x n matrix binary matrix. 0 - free cell, 1 -
// occupied
int grid[GRID_WIDTH][GRID_HEIGHT] = {0};
// Array of rows that need to be destroyed
int to_destroy[GRID_HEIGHT] = {0};

// Array of blocks in the current shape
// Each value pair corresponds to the shift from the shape position over x and y
// axis
int current_shape[8] = {0};
int current_shape_type;

// Current rotation identifier 0-4
int current_rotation = 0;
// Current shape coordinates
int current_x = 0, current_y = 0;

// Represent shapes as an array of 8 ints.
// Each int pair represents the shift from the shape position over x and y axis
int SHAPES[7][8] = {
    {0, 0, 1, 0, 0, 1, 1, 1},   // O
    {0, 0, -1, 0, 1, 0, 0, 1},  // T
    {0, 0, 0, -1, 0, 1, 1, 1},  // L
    {0, 0, 0, -1, 0, 1, -1, 1}, // J
    {0, 0, 0, -1, 0, 1, 0, 2},  // I
    {0, 0, 1, 0, 0, 1, -1, 1},  // S
    {0, 0, -1, 0, 0, 1, 1, 1},  // Z
};

void restart_game() {
  for (int i = 0; i < GRID_WIDTH; i++) {
    for (int j = 0; j < GRID_HEIGHT; j++) {
      grid[i][j] = 0;
    }
  };

  game_over = 0;

  SDL_Delay(300);
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

  current_x = GRID_WIDTH / 2;
  current_y = 0;
  current_rotation = 0;
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
    if (!grid[i][y]) {
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
      grid[x][y] = 1;
    }

    if (!row_is_full(y)) {
      if (y <= 0) {
        end_game();
      }
    }
  }

  iteration = 0;
  score += SCORE_SINGLE;
  current_action = MOVE_DOWN;
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
  if (current_shape_type == 0) {
    return; // O-shape should not be rotated
  }

  int temp[8] = {0};

  int multiplier;
  int x, y;

  if (current_rotation % 2 == 0) {
    multiplier = -1;
  } else {
    multiplier = 1;
  }

  for (int i = 0; i < 4; i++) {
    temp[i * 2] = current_shape[i * 2 + 1] * multiplier;
    temp[i * 2 + 1] = current_shape[i * 2] * multiplier;

    x = temp[i * 2] + current_x;
    y = temp[i * 2 + 1] + current_y;

    if (detect_collision(x, y)) {
      return;
    }
  };

  for (int i = 0; i < 8; i++) {
    current_shape[i] = temp[i];
  }

  current_rotation += 1;
  if (current_rotation == 4) {
    current_rotation = 0;
  }
}

void move_side(int n) {
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
  // Move only once in WAIT times
  if (iteration < WAIT) {
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

void handle_current_action() {
  switch (current_action) {
  case MOVE_DOWN:
    move_down();
    break;
  case MOVE_LEFT:
    move_side(-1);
    current_action = MOVE_DOWN;
    break;
  case MOVE_RIGHT:
    move_side(1);
    current_action = MOVE_DOWN;
    break;
  case ROTATE:
    rotate_shape();
    current_action = MOVE_DOWN;
    break;
  case FALL:
    iteration = WAIT;
    move_down();
    break;
  }
}

void update_frame() {
  if (game_over) {
    render_game_over_message();
    return;
  }

  clear_screen();

  for (int i = 0; i < GRID_WIDTH; ++i) {
    for (int j = 0; j < GRID_HEIGHT; ++j) {
      if (grid[i][j]) {
        draw_block(i, j, 0);
      } else {
        draw_block(i, j, 1);
      }
    }
  }

  int x, y;

  for (int i = 0; i < 4; i++) {
    x = current_shape[i * 2] + current_x;
    y = current_shape[i * 2 + 1] + current_y;

    draw_block(x, y, 0);
  }

  present_screen();
}

void init_game() {
  srand(time(NULL)); // seed the random number generator

  spawn_shape();
  init_graphics();
}

void game_loop() {
  handle_current_action();
  clean_destroyed_blocks();
  update_frame();
  iteration++;
  SDL_Delay(16);
}

void terminate_game() { release_resources(); }
