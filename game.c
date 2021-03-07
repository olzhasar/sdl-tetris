#include<stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include "game.h"

int x = GRID_WIDTH / 2;
int y = 0;
int y_pos = 0;
int grid[GRID_WIDTH][GRID_HEIGHT] = {0};
int velocity = VELOCITY;
bool increase_requested = false;


void DrawSquare(int x, int y, SDL_Renderer *rend) {
  SDL_Rect r;
  r.x = x;
  r.y = y;
  r.w = BLOCK_SIZE;
  r.h = BLOCK_SIZE;

  SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
  SDL_RenderFillRect(rend, &r);
}

void MoveY() {
  if (y >= GRID_HEIGHT - 1 || grid[x][y+1] == 1) {
    grid[x][y] = 1; // save current position
    y = 0; // start from top
    y_pos = 0;
    velocity = VELOCITY;
    x = GRID_WIDTH / 2;
  }

  y_pos += velocity;

  if (y_pos % BLOCK_SIZE == 0) {
    y += 1;
    if (increase_requested) {
      velocity = BLOCK_SIZE;
      increase_requested = false;
    }
  }
}


void MoveX(int value) {
  int new_x = x + value;

  if (new_x < 0 || new_x >= GRID_WIDTH || grid[new_x][y] == 1) {
    return;
  }

  x = new_x;
}


void RotateFigure() {
  /* IMPLEMENT ME!!! */
}

void HandleKeyDown(SDL_Event event) {
  switch( event.key.keysym.sym ) {
    case SDLK_LEFT:
    case SDLK_a:
      MoveX(-1);
      break;
    case SDLK_RIGHT:
    case SDLK_d:
      MoveX(1);
      break;
    case SDLK_DOWN:
    case SDLK_s:
      increase_requested = true;
      break;
    case SDLK_SPACE:
      RotateFigure();
  }

}

void GameLoop(SDL_Renderer *rend) {
  SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);

  SDL_RenderClear(rend);

  DrawSquare(x * BLOCK_SIZE, y_pos, rend);

  int i;
  int j;

  for (i = 0; i < GRID_WIDTH; ++i) {
    for (j = 0; j < GRID_HEIGHT; ++j) {
      if (grid[i][j] == 1) {
	DrawSquare(i * BLOCK_SIZE, j * BLOCK_SIZE, rend);
      }
    }
  }

  SDL_RenderPresent(rend);

  SDL_Delay(FRAME_DELAY);

  MoveY();
}
