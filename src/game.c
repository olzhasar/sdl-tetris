#include "game.h"

enum Action { MOVE_LEFT, MOVE_RIGHT, MOVE_DOWN, ROTATE, FALL };

int grid[GRID_WIDTH][GRID_HEIGHT] = {0};
int game_over = 0;
int to_erase[GRID_HEIGHT] = {0};
enum Action current_action = MOVE_DOWN;

struct Block {
  int x;
  int y;
};

struct Shape {
  struct Block blocks[SHAPE_SIZE];
  int rotatable;
};

struct Shape shapes[N_SHAPES] = {
    {
        {
            {0, 0},
            {0, 1},
            {-1, 1},
            {1, 1},
        },
        1,
    },
    {
        {
            {0, -1},
            {0, 0},
            {0, 1},
            {1, 1},
        },
        1,
    },
    {
        {
            {0, -1},
            {0, 0},
            {0, 1},
            {0, 2},
        },
        1,
    },
    {
        {
            {0, 0},
            {1, 0},
            {0, 1},
            {1, 1},
        },
        0,
    },
    {
        {
            {0, -1},
            {0, 0},
            {1, 0},
            {1, 1},
        },
        1,
    },
};

struct ActiveShape {
  struct Shape shape;
  int x, y;
  int rotation;
};

struct ActiveShape active_shape;

void restart_game() {
  int i, j;

  for (i = 0; i < GRID_WIDTH; i++) {
    for (j = 0; j < GRID_HEIGHT; j++) {
      grid[i][j] = 0;
    }
  };

  game_over = 0;
}

void end_game() {
  game_over = 1;
  clear_screen();
}

void refresh_active_shape() {
  active_shape.shape = shapes[rand() % N_SHAPES];
  active_shape.x = GRID_WIDTH / 2;
  active_shape.y = 0;
  active_shape.rotation = 0;
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
    if (to_erase[i]) {
      to_erase[i] = 0;
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

  to_erase[y] = 1;
  return 1;
}

void save_active_shape() {
  int i;
  int x, y;

  for (i = 0; i < SHAPE_SIZE; i++) {
    x = active_shape.x + active_shape.shape.blocks[i].x;
    y = active_shape.y + active_shape.shape.blocks[i].y;
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
  current_action = MOVE_DOWN;

  if (!active_shape.shape.rotatable) {
    return;
  }

  struct Block new_blocks[SHAPE_SIZE];

  int i;
  int multiplier;
  int x_pos, y_pos;

  struct Block *block;

  if (active_shape.rotation % 2 == 0) {
    multiplier = -1;
  } else {
    multiplier = 1;
  }

  for (i = 0; i < SHAPE_SIZE; i++) {
    block = &active_shape.shape.blocks[i];

    new_blocks[i].x = block->y * multiplier;
    new_blocks[i].y = block->x * multiplier;

    x_pos = active_shape.x + new_blocks[i].x;
    y_pos = active_shape.y + new_blocks[i].y;

    if (detect_collision(x_pos, y_pos)) {
      return;
    }
  };

  for (i = 0; i < SHAPE_SIZE; i++) {
    block = &active_shape.shape.blocks[i];

    block->x = new_blocks[i].x;
    block->y = new_blocks[i].y;
  }

  if (active_shape.rotation == 3) {
    active_shape.rotation = 0;
  } else {
    active_shape.rotation += 1;
  }
}

void move_side(int n) {
  current_action = MOVE_DOWN;

  int x, y;

  for (int i = 0; i < SHAPE_SIZE; i++) {
    struct Block block = active_shape.shape.blocks[i];
    x = active_shape.x + block.x + n;
    y = active_shape.y + block.y;

    if (detect_collision(x, y)) {
      return;
    }
  }

  active_shape.x += n;
}

void move_down(int delay) {
  int i;
  int x, y;

  struct Block block;

  for (i = 0; i < SHAPE_SIZE; i++) {
    block = active_shape.shape.blocks[i];
    x = active_shape.x + block.x;
    y = active_shape.y + block.y + 1;

    if (detect_collision(x, y)) {
      save_active_shape();
      refresh_active_shape();
      return;
    }
  }

  SDL_Delay(delay);
  active_shape.y += 1;
}

void handle_current_action() {
  switch (current_action) {
  case MOVE_DOWN:
    move_down(FALL_DELAY);
    break;
  case MOVE_LEFT:
    move_side(-1);
    break;
  case MOVE_RIGHT:
    move_side(1);
    break;
  case ROTATE:
    rotate_shape();
    break;
  case FALL:
    move_down(0);
    break;
  }
}

void handle_key_down(SDL_Event event) {
  switch (event.key.keysym.sym) {
  case SDLK_LEFT:
  case SDLK_a:
    current_action = MOVE_LEFT;
    break;
  case SDLK_RIGHT:
  case SDLK_d:
    current_action = MOVE_RIGHT;
    break;
  case SDLK_DOWN:
  case SDLK_s:
    current_action = FALL;
    break;
  case SDLK_SPACE:
    current_action = ROTATE;
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
  struct Block block;

  for (int i = 0; i < SHAPE_SIZE; ++i) {
    block = active_shape.shape.blocks[i];
    x = active_shape.x + block.x;
    y = active_shape.y + block.y;

    draw_block(x, y, 0);
  }

  present_screen();
}

void prepare_game() {
  refresh_active_shape();
  init_graphics();
}

void game_loop() {
  handle_current_action();
  update_frame();
  clean_destroyed_blocks();
  SDL_Delay(16);
}

void terminate_game() { release_resources(); }
