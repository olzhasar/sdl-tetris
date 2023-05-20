#include "graphics.h"

int init_graphics() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("error initializing SDL: %s\\n", SDL_GetError());
    return 1;
  }

  win =
      SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE, 0);

  if (!win) {
    printf("error creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  rend = SDL_CreateRenderer(win, -1, render_flags);
  if (!rend) {
    printf("error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  SDL_CreateRGBSurface(0, GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE, 32,
                       0, 0, 0, 0);

  // Game over text

  TTF_Init();
  TTF_Font *Font = TTF_OpenFont("assets/font.ttf", 18);
  SDL_Color White = {255, 255, 255};

  SDL_Surface *surfaceMessage =
      TTF_RenderText_Solid(Font, "Game Over. Press any key to restart", White);
  Message = SDL_CreateTextureFromSurface(rend, surfaceMessage);

  Message_rect.x = BLOCK_SIZE;
  Message_rect.y = (GRID_HEIGHT / 2 - 1) * BLOCK_SIZE;
  Message_rect.w = (GRID_WIDTH - 2) * BLOCK_SIZE;
  Message_rect.h = BLOCK_SIZE / 2;

  SDL_RenderCopy(rend, Message, NULL, &Message_rect);

  return 0;
}

void draw_block(int x, int y, bool empty) {
  SDL_Rect outer;
  SDL_Rect inner;

  outer.x = x * BLOCK_SIZE;
  outer.y = y * BLOCK_SIZE;
  outer.w = BLOCK_SIZE;
  outer.h = BLOCK_SIZE;

  inner.x = x * BLOCK_SIZE + 1;
  inner.y = y * BLOCK_SIZE + 1;
  inner.w = BLOCK_SIZE - 2;
  inner.h = BLOCK_SIZE - 2;

  SDL_SetRenderDrawColor(rend, 0xdd, 0xdd, 0xdd, 0xff);
  SDL_RenderFillRect(rend, &outer);

  if (!empty) {
    SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0xff, 0xFF);
  } else {
    SDL_SetRenderDrawColor(rend, 0xcc, 0xcc, 0xcc, 0xFF);
  }
  SDL_RenderFillRect(rend, &inner);
}

void clear_screen() {
  SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
  SDL_RenderClear(rend);
}

void present_screen() { SDL_RenderPresent(rend); }

void release_resources() {
  SDL_FreeSurface(surfaceMessage);
  SDL_DestroyTexture(Message);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();
}
