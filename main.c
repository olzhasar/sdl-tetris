#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include "game.h"


int main(void) {
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

    GameLoop(rend);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
	close_requested = 1;
      }
      if (event.type == SDL_KEYDOWN) {
	if (event.key.keysym.sym == SDLK_ESCAPE) {
	  close_requested = 1;
	} else {
	  HandleKeyDown(event);
	}
      }
    }
  }
  
  // clean up resources before exiting
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
