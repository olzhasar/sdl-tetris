#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
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

  SDL_CreateRGBSurface(0, GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE, 32, 0, 0, 0, 0);

  // Game over text

  TTF_Init();
  TTF_Font* Font = TTF_OpenFont("assets/font.ttf", 18);
  SDL_Color White = {255, 255, 255};

  SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Font, "Game Over. Press any key to restart", White);
  SDL_Texture* Message = SDL_CreateTextureFromSurface(rend, surfaceMessage);

  SDL_Rect Message_rect;
  Message_rect.x = BLOCK_SIZE;
  Message_rect.y = (GRID_HEIGHT / 2 - 1) * BLOCK_SIZE;
  Message_rect.w = (GRID_WIDTH - 2) * BLOCK_SIZE;
  Message_rect.h = BLOCK_SIZE / 2;

  SDL_RenderCopy(rend, Message, NULL, &Message_rect);

  PrepareForStart();

  int close_requested = 0;

  while (!close_requested) {
    SDL_Event event;

    if (!game_over) {
      GameLoop(rend);
    } else {
      SDL_RenderClear(rend);
      SDL_RenderCopy(rend, Message, NULL, &Message_rect);
      SDL_RenderPresent(rend);
    }

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
	close_requested = 1;
      }
      if (event.type == SDL_KEYDOWN) {
	if (event.key.keysym.sym == SDLK_ESCAPE) {
	  close_requested = 1;
	} else {
	  if (!game_over) {
	    HandleKeyDown(event);
	  } else {
	    RestartGame();
	  }
	}
      }
    }
  }
  
  // clean up resources before exiting
  SDL_FreeSurface(surfaceMessage);
  SDL_DestroyTexture(Message);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
