#include "game.h"
#include <stdlib.h>
#include <string.h>

static int rows_to_destroy = 0; // a bitmask

static unsigned int lines_cleared = 0;

static const int MAX_LEVEL_PERIOD = 15;
static const unsigned int LEVEL_PERIODS[15] = {
    800, 700, 620, 540, 460, 380, 300, 220, 150, 110, 85, 65, 50, 35, 20};

static const int SOFT_FALL_PERIOD = 30;

static int shape_bag[N_SHAPES];
static int shape_bag_idx = N_SHAPES - 1;

// Represent shapes as arrays of 8 ints.
// Each int pair represents the shift from the shape central position over x and y axis
static int SHAPES[N_SHAPES][8] = {
    {0, 0, 1, 0, 0, 1, 1, 1},   // O
    {0, 0, -1, 0, 1, 0, 0, 1},  // T
    {0, 0, 0, -1, 0, 1, 1, 1},  // L
    {0, 0, 0, -1, 0, 1, -1, 1}, // J
    {0, -1, 0, 0, 0, 1, 0, 2},  // I
    {0, 0, 1, 0, 0, 1, -1, 1},  // S
    {0, 0, -1, 0, 0, 1, 1, 1},  // Z
};

static const int SCORE_EARN_SINGLE = 1;
static const int SCORE_EARN_LINE = 100;

static int get_curr_fall_period(game_state_t *state) {
  if (state->level >= MAX_LEVEL_PERIOD) {
    return LEVEL_PERIODS[MAX_LEVEL_PERIOD - 1];
  }
  return LEVEL_PERIODS[state->level];
};

void reset_fall_period(game_state_t *state) {
  state->fall_period_ms = get_curr_fall_period(state);
}

void update_fall_period(game_state_t *state, int new) {
  int calculated = get_curr_fall_period(state);
  state->fall_period_ms = calculated < new ? calculated : new;
}

int get_next_shape_kind() {
  shape_bag_idx = (shape_bag_idx + 1) % N_SHAPES;

  if (shape_bag_idx == 0) {
    for (int i = 0; i < N_SHAPES; i++) {
      shape_bag[i] = i;
    }

    // Fisher Yates shuffle
    for (int i = N_SHAPES - 1; i > 0; i--) {
      int j = rand() % (i + 1);
      int temp = shape_bag[i];
      shape_bag[i] = shape_bag[j];
      shape_bag[j] = temp;
    }
  }

  return shape_bag[shape_bag_idx];
}

void spawn_shape(game_state_t *state) {
  state->changed = 1;

  state->current_shape_kind = get_next_shape_kind();

  for (int i = 0; i < 8; i++) {
    state->current_shape[i] = SHAPES[state->current_shape_kind][i];
  }

  state->current_x = GRID_WIDTH / 2;

  // Check for top collisions with existing blocks in the grid
  // If we spot any collision, we'll start with negative current_y
  int x, y;
  for (state->current_y = -2; state->current_y < 0; state->current_y++) {
    for (int i = 0; i < 4; i++) {
      x = state->current_shape[i * 2] + state->current_x;
      y = state->current_shape[i * 2 + 1] + state->current_y + 1;

      if (y >= 0 && state->grid[y][x] != 0) {
        return;
      }
    }
  };
}

void restart_game(game_state_t *state) {
  memset(state, 0, sizeof(*state));
  state->changed = 1;

  lines_cleared = 0;
  state->fall_elapsed_ms = 0;
  state->fall_period_ms = LEVEL_PERIODS[0];

  spawn_shape(state);
}

void destroy_row(game_state_t *state, int target_row) {
  for (int r = target_row; r >= 0; r--) {
    for (int c = 0; c < GRID_WIDTH; c++) {
      if (r > 0) {
        state->grid[r][c] = state->grid[r - 1][c];
      } else {
        state->grid[r][c] = 0;
      }
    }
  }
  lines_cleared++;
  if (lines_cleared % 10 == 0) {
    state->level++;
  }
}

void clean_destroyed_blocks(game_state_t *state) {
  int count = 0;

  for (int r = 0; r < GRID_HEIGHT; r++) {
    if (rows_to_destroy & (1 << r)) {
      count++;

      rows_to_destroy ^= (1 << r);
      for (int c = 0; c < GRID_WIDTH; c++) {
        state->grid[r][c] = 0;
      }
      destroy_row(state, r);
    }
  }

  if (count) {
    state->score += SCORE_EARN_LINE * (1 + 2 * (count - 1));
  }
}

int row_is_full(game_state_t *state, int r) {
  if (r < 0) {
    return 0;
  }

  if (rows_to_destroy & (1 << r)) {
    return 1;
  }

  for (int c = 0; c < GRID_WIDTH; c++) {
    if (state->grid[r][c] == 0) {
      return 0;
    }
  }

  rows_to_destroy |= (1 << r);
  return 1;
}

void lock_shape(game_state_t *state) {
  int x, y;

  int to_destroy = 0;

  for (int i = 0; i < 4; i++) {
    x = state->current_shape[i * 2] + state->current_x;
    y = state->current_shape[i * 2 + 1] + state->current_y;

    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
      state->grid[y][x] = state->current_shape_kind + 1;
    }

    if (row_is_full(state, y)) {
      to_destroy++;
    } else {
      if (y <= 0) {
        state->game_over = 1;
        state->changed = 1;
      }
    }
  }

  if (to_destroy) {
    clean_destroyed_blocks(state);
  }

  state->fall_elapsed_ms = 0;
  state->score += SCORE_EARN_SINGLE;
  reset_fall_period(state);

  if (!state->game_over) {
    spawn_shape(state);
  }
}

int detect_collision(game_state_t *state, int x, int y) {
  if (x < 0 || x >= GRID_WIDTH) {
    return 1;
  }

  if (y < 0) { // y above the top is fine when spawning
    return 0;
  }

  return (y >= GRID_HEIGHT || state->grid[y][x]);
}

void rotate_shape(game_state_t *state) {
  reset_fall_period(state);

  if (state->current_shape_kind == 0) {
    return; // O-shape should not be rotated
  }

  state->changed = 1;

  int temp[8] = {0};

  int x, y;

  for (int i = 0; i < 4; i++) {
    temp[i * 2] = state->current_shape[i * 2 + 1];
    temp[i * 2 + 1] = -state->current_shape[i * 2];

    x = temp[i * 2] + state->current_x;
    y = temp[i * 2 + 1] + state->current_y;

    if (detect_collision(state, x, y)) {
      return;
    }
  };

  for (int i = 0; i < 8; i++) {
    state->current_shape[i] = temp[i];
  }
}

void move_side(game_state_t *state, int direction) {
  reset_fall_period(state);

  int x, y;

  for (int i = 0; i < 4; i++) {
    x = state->current_shape[i * 2] + state->current_x + direction;
    y = state->current_shape[i * 2 + 1] + state->current_y;

    if (detect_collision(state, x, y)) {
      return;
    }
  }

  state->current_x += direction;
  state->changed = 1;
}

void fall(game_state_t *state, unsigned int elapsed_ms, int hard_drop) {
  state->fall_elapsed_ms += elapsed_ms;
  if (state->fall_elapsed_ms < state->fall_period_ms && !hard_drop) {
    return;
  }
  state->fall_elapsed_ms = 0;

  int x, y;

  do {
    for (int i = 0; i < 4; i++) {
      x = state->current_shape[i * 2] + state->current_x;
      y = state->current_shape[i * 2 + 1] + state->current_y + 1;

      if (detect_collision(state, x, y)) {
        return lock_shape(state);
      }
    }
    state->current_y += 1;

  } while (hard_drop);

  state->changed = 1;
}

void handle_input_event(game_state_t *state, input_event_t event) {
  switch (event) {
  case LEFT:
    return move_side(state, -1);
  case RIGHT:
    return move_side(state, 1);
  case ROTATE:
    return rotate_shape(state);
  case SOFT_DROP:
    return update_fall_period(state, SOFT_FALL_PERIOD);
  case HARD_DROP:
    return fall(state, 0, 1);
  default:
    return;
  }
}

game_state_t game_state_new() {
  game_state_t state = {
      .game_over = 0,
      .level = 0,
      .grid = {0},
      .current_shape = {0},
      .current_shape_kind = 0,
      .fall_elapsed_ms = 0,
      .fall_period_ms = LEVEL_PERIODS[0],
  };

  spawn_shape(&state);

  return state;
}

void game_state_update(game_state_t *state, input_event_t event,
                       unsigned int elapsed_ms) {
  state->changed = 0;
  if (state->game_over) {
    if (event == START) {
      restart_game(state);
    }
  } else {
    handle_input_event(state, event);
    fall(state, elapsed_ms, 0);
  }
}
