#include<stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include "game.h"

#define N_SHAPES 5
#define SHAPE_SIZE 4
#define FRAME_DELAY 50
#define VELOCITY 10

int grid[GRID_WIDTH][GRID_HEIGHT] = {0};
int velocity = VELOCITY;
bool increase_requested = false;

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
      {0, 0},
      {0, 1},
      {0, 2},
      {1, 2},
    },
    1,
  },
  {
    {
      {0, 0},
      {0, 1},
      {0, 2},
      {0, 3},
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
      {0, 0},
      {0, 1},
      {1, 1},
      {1, 2},
    },
    1,
  },
};


struct ActiveShape {
  struct Shape shape;
  int x, y;
  int y_pos;
  int rotation;
};


struct ActiveShape active_shape;


void RefreshActiveShape() {
  active_shape.shape = shapes[rand() % N_SHAPES];
  active_shape.x = GRID_WIDTH / 2;
  active_shape.y = 0;
  active_shape.y_pos = 0;
  active_shape.rotation = 0;
}


void SaveActiveShape() {
  int i;
  int x, y;

  for (i = 0; i < SHAPE_SIZE; i++) {
    x = active_shape.x + active_shape.shape.blocks[i].x;
    y = active_shape.y + active_shape.shape.blocks[i].y;
    grid[x][y] = 1;
  }
}


void RotateShape() {
  if (active_shape.shape.rotatable == 0) {
    return;
  }

  int new_x;
  int i;
  int multiplier;
  struct Block *block;

  if (active_shape.rotation % 2 == 0) {
    multiplier = -1;
  } else {
    multiplier = 1;
  }

  for (i=0; i < SHAPE_SIZE; i++) {
    block = &active_shape.shape.blocks[i];
    new_x = block->y * multiplier;
    block->y = block->x * multiplier;
    block->x = new_x;
  };

  if (active_shape.rotation == 3) {
    active_shape.rotation = 0;
  } else {
    active_shape.rotation += 1;
  }
}


void DrawBlock(int x, int y, SDL_Renderer *rend) {
  SDL_Rect r;
  r.x = x;
  r.y = y;
  r.w = BLOCK_SIZE;
  r.h = BLOCK_SIZE;

  SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
  SDL_RenderFillRect(rend, &r);
}


int DetectCollision(int x, int y) {
  if (x < 0 || x >= GRID_WIDTH) {
    return 1;
  }

  if (y < 0 || y >= GRID_HEIGHT) {
    return 1;
  }

  if (grid[x][y] == 1) {
    return 1;
  }

  return 0;
}


void MoveSide(int n) {
  int collision = 0;
  int i;
  int x, y;

  struct Block block;

  for (i = 0; i < SHAPE_SIZE; i++) {
    block = active_shape.shape.blocks[i];
    x = active_shape.x + block.x + n;
    y = active_shape.y + block.y;

    if (DetectCollision(x, y) == 1) {
      collision = 1;
      break;
    }
  }

  if (collision == 0) {
    active_shape.x += n;
  }
}


void MoveDown() {
  int collision = 0;
  int i;
  int x, y;

  struct Block block;

  for (i = 0; i < SHAPE_SIZE; i++) {
    block = active_shape.shape.blocks[i];
    x = active_shape.x + block.x;
    y = active_shape.y + block.y + 1;

    if (DetectCollision(x, y) == 1) {
      collision = 1;
      break;
    }
  }

  if (collision == 1) {
    SaveActiveShape();
    RefreshActiveShape();
    return;
  }

  active_shape.y_pos += velocity;
  if (active_shape.y_pos % BLOCK_SIZE == 0) {
    active_shape.y += 1;
  }

}


void HandleKeyDown(SDL_Event event) {
  switch( event.key.keysym.sym ) {
    case SDLK_LEFT:
    case SDLK_a:
      MoveSide(-1);
      break;
    case SDLK_RIGHT:
    case SDLK_d:
      MoveSide(1);
      break;
    case SDLK_DOWN:
    case SDLK_s:
      increase_requested = true;
      break;
    case SDLK_SPACE:
      RotateShape();
  }

}


void DrawActiveShape(SDL_Renderer *rend) {
  int i;
  int x, y;
  struct Block block;

  for (i = 0; i < 4; ++i) {
    block = active_shape.shape.blocks[i];
    x = (active_shape.x + block.x) * BLOCK_SIZE;
    y = active_shape.y_pos + block.y * BLOCK_SIZE;

    DrawBlock(x, y, rend);
  }
}

void DrawGridBlocks(SDL_Renderer *rend) {
  int i, j;

  for (i = 0; i < GRID_WIDTH; ++i) {
    for (j = 0; j < GRID_HEIGHT; ++j) {
      if (grid[i][j] == 1) {
	DrawBlock(i * BLOCK_SIZE, j * BLOCK_SIZE, rend);
      }
    }
  }
}


void PrepareForStart() {
  RefreshActiveShape();
}


void GameLoop(SDL_Renderer *rend) {
  SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);

  SDL_RenderClear(rend);

  DrawActiveShape(rend);
  DrawGridBlocks(rend);

  SDL_RenderPresent(rend);

  SDL_Delay(FRAME_DELAY);

  MoveDown();
}
