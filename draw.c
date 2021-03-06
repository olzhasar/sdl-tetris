#include<stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define BLOCK_SIZE 50
#define GRID_WIDTH 11
#define GRID_HEIGHT 15
#define FRAME_DELAY 50

int y = 0;

struct Square {
  int x;
  int y;
};

void draw_rect(struct Square sq, SDL_Renderer *rend) {
  SDL_Rect r;
  r.x = sq.x;
  r.y = sq.y;
  r.w = BLOCK_SIZE;
  r.h = BLOCK_SIZE;

  printf("X: %d, Y: %d\n", sq.x, sq.y);

  SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
  SDL_RenderFillRect(rend, &r);

}

void game_loop(SDL_Renderer *rend) {
  struct Square sq;

  sq.x = 5 * BLOCK_SIZE;
  sq.y = y;

  SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);

  SDL_RenderClear(rend);

  draw_rect(sq, rend);

  SDL_RenderPresent(rend);

  SDL_Delay(FRAME_DELAY);

  y += 5;
}

int draw_figures(void) {
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_VIDEO) != 0) {
    printf("error initializing SDL: %s\\n", SDL_GetError());
    return 1;
  }

  SDL_Window* win = SDL_CreateWindow("Tetris",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      GRID_WIDTH * BLOCK_SIZE,
      GRID_HEIGHT * BLOCK_SIZE,
      0);

  if (!win) {
    printf("error creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
  if (!rend) {
    printf("error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  int close_requested = 0;

  while (!close_requested) {
    SDL_Event event;

    game_loop(rend);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
	close_requested = 1;
      }
    }
  }
  
  // clean up resources before exiting
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
