#include "game.h"

enum Action current_action = MOVE_DOWN;
int game_over = 0;

// Grid is represented as m x n matrix binary matrix. 0 - free cell, 1 -
// occupied
int grid[GRID_WIDTH][GRID_HEIGHT] = {0};
// Array of rows that need to be destroyed
int to_destroy[GRID_HEIGHT] = {0};

// Array of blocks in the current shape
// Each value pair corresponds to the shift from the shape position over x and y
// axis
int current_shape[8] = {0};

// Current rotation identifier 0-4
int current_rotation = 0;
// Current shape coordinates
int current_x = 0, current_y = 0;

// Represent shapes as an array of 8 ints.
// Each int pair represents the shift from the shape position over x and y axis
int SHAPES[N_SHAPES][8] = {
    {0, 0, 0, 1, -1, 1, 1, 1}, {0, -1, 0, 0, 0, 1, 1, 1},
    {0, -1, 0, 0, 0, 1, 0, 2}, {0, 0, 1, 0, 0, 1, 1, 1},
    {0, -1, 0, 0, 1, 0, 1, 1},
};

void restart_game() {
  for (int i = 0; i < GRID_WIDTH; i++) {
    for (int j = 0; j < GRID_HEIGHT; j++) {
      grid[i][j] = 0;
    }
  };

  game_over = 0;
}

void end_game() {
  game_over = 1;
  clear_screen();
}

void spawn_shape() {
  int n = rand() % N_SHAPES;

  for (int i = 0; i < N_SHAPES; i++) {
    current_shape[i * 2] = SHAPES[n][i * 2];
    current_shape[i * 2 + 1] = SHAPES[n][i * 2 + 1];
  }

  current_x = GRID_WIDTH / 2;
  current_y = 0;
  current_rotation = 0;
}

void shift_blocks_down(int row) {
  int i, j;

  for (j = row; j > 0; j--) {
    for (i = 0; i < GRID_WIDTH; i++) {
      grid[i][j] = grid[i][j - 1];
    }
  }
}

void clean_destroyed_blocks() {
  for (int i = 0; i < GRID_HEIGHT; i++) {
    if (to_destroy[i]) {
      to_destroy[i] = 0;
      for (int j = 0; j < GRID_WIDTH; j++) {
        grid[i][j] = 0;
      }
      shift_blocks_down(i);
    }
  }
}

int row_is_full(int y) {
  int i;

  for (i = 0; i < GRID_WIDTH; i++) {
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
    grid[x][y] = 1;

    if (!row_is_full(y)) {
      if (y <= 0) {
        end_game();
      }
    }
  }

  current_action = MOVE_DOWN;
}

int detect_collision(int x, int y) {
  if (x < 0 || x >= GRID_WIDTH) {
    return 1;
  }

  if (y < 0 || y >= GRID_HEIGHT) {
    return 1;
  }

  if (grid[x][y]) {
    return 1;
  }

  return 0;
}

void rotate_shape() {
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
      current_action = MOVE_DOWN;
      return;
    }
  }

  current_x += n;
}

void move_down(int delay) {
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

  SDL_Delay(delay);
  current_y += 1;
}

void handle_current_action() {
  switch (current_action) {
  case MOVE_DOWN:
    move_down(FALL_DELAY);
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
    move_down(0);
    break;
  }
}

void update_frame() {
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
  spawn_shape();
  init_graphics();
}

void game_loop() {
  handle_current_action();
  update_frame();
  clean_destroyed_blocks();
  SDL_Delay(16);
}

void terminate_game() { release_resources(); }
