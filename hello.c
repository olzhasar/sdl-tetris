#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>


int main(void) {
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_VIDEO) != 0) {
    printf("error initializing SDL: %s\\n", SDL_GetError());
    return 1;
  }

  SDL_Window* win = SDL_CreateWindow("Tetris",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      640, 480, 0);

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

  SDL_Rect r;
  r.x = 50;
  r.y = 50;
  r.w = 50;
  r.h = 50;

  SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
  SDL_RenderFillRect(rend, &r);

  SDL_RenderPresent(rend);

  SDL_Delay(5000);
  
  // clean up resources before exiting
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();
}
