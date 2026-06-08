#include "game.h"
#include <stdlib.h>
#include <string.h>

// Array of rows that need to be destroyed
static int to_destroy[GRID_HEIGHT] = {0};

static unsigned int lines_cleared = 0;

static const int MAX_LEVEL_FREQ = 15;
static const int LEVEL_PERIODS[15] = {48, 43, 38, 33, 28, 23, 18, 13,
                                      8,  6,  5,  4,  3,  2,  1};

static unsigned int fall_tick = 0;
static unsigned int fall_period = LEVEL_PERIODS[0];

static const int SOFT_FALL_PERIOD = 3;
static const int HARD_FALL_PERIOD = 1;

// Represent shapes as arrays of 8 ints.
// Each int pair represents the shift from the shape position over x and y axis
static int SHAPES[N_SHAPES][8] = {
    {0, 0, 1, 0, 0, 1, 1, 1},   // O
    {0, 0, -1, 0, 1, 0, 0, 1},  // T
    {0, 0, 0, -1, 0, 1, 1, 1},  // L
    {0, 0, 0, -1, 0, 1, -1, 1}, // J
    {0, 0, 0, -1, 0, 1, 0, 2},  // I
    {0, 0, 1, 0, 0, 1, -1, 1},  // S
    {0, 0, -1, 0, 0, 1, 1, 1},  // Z
};

static const int SCORE_EARN_SINGLE = 1;
static const int SCORE_EARN_LINE = 100;

static int get_curr_fall_freq(game_state_t *state) {
  if (state->level >= MAX_LEVEL_FREQ) {
    return LEVEL_PERIODS[MAX_LEVEL_FREQ - 1];
  }
  return LEVEL_PERIODS[state->level];
};

void reset_fall_freq(game_state_t *state) {
  fall_period = get_curr_fall_freq(state);
}

void update_fall_freq(game_state_t *state, int new) {
  int calculated = get_curr_fall_freq(state);
  if (calculated < new) {
    fall_period = calculated;
  } else {
    fall_period = new;
  }
}

void spawn_shape(game_state_t *state) {
  state->changed = 1;

  state->current_shape_kind = rand() % N_SHAPES;

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

      if (y >= 0 && state->grid[x][y] != 0) {
        return;
      }
    }
  };
}

void restart_game(game_state_t *state) {
  memset(state, 0, sizeof(*state));
  state->changed = 1;

  lines_cleared = 0;
  fall_tick = 0;
  fall_period = LEVEL_PERIODS[0];

  spawn_shape(state);
}

void destroy_row(game_state_t *state, int row) {
  for (int j = row; j > 0; j--) {
    for (int i = 0; i < GRID_WIDTH; i++) {
      state->grid[i][j] = state->grid[i][j - 1];
    }
  }
  lines_cleared++;
  if (lines_cleared % 10 == 0) {
    state->level++;
  }
}

void clean_destroyed_blocks(game_state_t *state) {
  int count = 0;

  for (int j = 0; j < GRID_HEIGHT; j++) {
    if (to_destroy[j]) {
      count++;

      to_destroy[j] = 0;
      for (int i = 0; i < GRID_WIDTH; i++) {
        state->grid[i][j] = 0;
      }
      destroy_row(state, j);
    }
  }

  if (count) {
    state->score += SCORE_EARN_LINE * (1 + 2 * (count - 1));
  }
}

int row_is_full(game_state_t *state, int y) {
  if (y < 0) {
    return 0;
  }

  if (to_destroy[y]) {
    return 1;
  }

  for (int i = 0; i < GRID_WIDTH; i++) {
    if (state->grid[i][y] == 0) {
      return 0;
    }
  }

  to_destroy[y] = 1;
  return 1;
}

void lock_shape(game_state_t *state) {
  int x, y;

  int to_destroy = 0;

  for (int i = 0; i < 4; i++) {
    x = state->current_shape[i * 2] + state->current_x;
    y = state->current_shape[i * 2 + 1] + state->current_y;

    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
      state->grid[x][y] = state->current_shape_kind + 1;
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

  fall_tick = 0;
  state->score += SCORE_EARN_SINGLE;
  reset_fall_freq(state);

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

  return (y >= GRID_HEIGHT || state->grid[x][y]);
}

void rotate_shape(game_state_t *state) {
  reset_fall_freq(state);

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
  reset_fall_freq(state);

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

void fall(game_state_t *state) {
  fall_tick = (fall_tick + 1) % fall_period;
  if (fall_tick != 0) {
    return;
  }

  int x, y;

  for (int i = 0; i < 4; i++) {
    x = state->current_shape[i * 2] + state->current_x;
    y = state->current_shape[i * 2 + 1] + state->current_y + 1;

    if (detect_collision(state, x, y)) {
      return lock_shape(state);
    }
  }

  state->current_y += 1;
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
  case HARD_DROP:
    return update_fall_freq(state, HARD_FALL_PERIOD);
  case SOFT_DROP:
    return update_fall_freq(state, SOFT_FALL_PERIOD);
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
  };

  spawn_shape(&state);
  return state;
}

void game_state_update(game_state_t *state, input_event_t event) {
  state->changed = 0;
  if (state->game_over) {
    if (event == START) {
      restart_game(state);
    }
  } else {
    handle_input_event(state, event);
    fall(state);
  }
}
