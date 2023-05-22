#include "graphics.h"
#include <assert.h>

SDL_Window *win;
SDL_Renderer *rend;
SDL_Surface *game_over_surface, *score_surface;
SDL_Texture *game_over_texture, *score_texture;
SDL_Rect game_over_rect, score_rect;

SDL_Color White = {0xff, 0xff, 0xff};
SDL_Color Gray = {0xcc, 0xcc, 0xcc};
TTF_Font *Font_18;
TTF_Font *Font_32;

int init_graphics() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("error initializing SDL: %s\\n", SDL_GetError());
    return 1;
  }

  win = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, (GRID_WIDTH + 5) * BLOCK_SIZE,
                         (GRID_HEIGHT + 2) * BLOCK_SIZE, 0);

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

  SDL_CreateRGBSurface(0, (GRID_WIDTH + 5) * BLOCK_SIZE,
                       (GRID_HEIGHT + 2) * BLOCK_SIZE, 32, 0, 0, 0, 0);

  // Game over text

  TTF_Init();
  Font_18 = TTF_OpenFont("src/assets/font.ttf", 18);
  Font_32 = TTF_OpenFont("src/assets/font.ttf", 32);

  return 0;
}

void render_score(int score) {
  char score_str[12];

  snprintf(score_str, 12, "%i", score);

  score_surface = TTF_RenderText_Solid(Font_32, score_str, Gray);
  score_texture = SDL_CreateTextureFromSurface(rend, score_surface);

  score_rect.x = (GRID_WIDTH + 3) * BLOCK_SIZE - score_surface->w / 2;
  score_rect.y = BLOCK_SIZE * 2 - score_surface->h / 2;
  score_rect.w = score_surface->w;
  score_rect.h = score_surface->h;

  SDL_RenderCopy(rend, score_texture, NULL, &score_rect);
}

void render_game_over_message() {
  game_over_surface = TTF_RenderText_Solid(
      Font_18, "Game Over. Press any key to restart", White);

  game_over_texture = SDL_CreateTextureFromSurface(rend, game_over_surface);

  game_over_rect.x = BLOCK_SIZE;
  game_over_rect.y = (GRID_HEIGHT / 2 - 1) * BLOCK_SIZE;
  game_over_rect.w = (GRID_WIDTH + 3) * BLOCK_SIZE;
  game_over_rect.h = BLOCK_SIZE / 2;

  SDL_RenderCopy(rend, game_over_texture, NULL, &game_over_rect);
  SDL_RenderPresent(rend);
}

void draw_block(int x, int y, bool empty) {
  if (y < 0) {
    return;
  }

  SDL_Rect outer;
  SDL_Rect inner;

  outer.x = (x + 1) * BLOCK_SIZE;
  outer.y = (y + 1) * BLOCK_SIZE;
  outer.w = BLOCK_SIZE;
  outer.h = BLOCK_SIZE;

  inner.x = (x + 1) * BLOCK_SIZE + 1;
  inner.y = (y + 1) * BLOCK_SIZE + 1;
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

void render_frame(int score) {
  render_score(score);
  SDL_RenderPresent(rend);
}

void release_resources() {
  SDL_FreeSurface(game_over_surface);
  SDL_FreeSurface(score_surface);
  SDL_DestroyTexture(game_over_texture);
  SDL_DestroyTexture(score_texture);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();
}
