#include "graphics.h"

#define WIN_TITLE "Tetris"
#define GAME_OVER_TEXT "Game Over. Press any key to restart"
#define FONT_PATH "src/assets/font.ttf"
#define SCORE_SIZE 7

const int WIN_WIDTH = (GRID_WIDTH + 5) * BLOCK_SIZE;
const int WIN_HEIGHT = (GRID_HEIGHT + 2) * BLOCK_SIZE;

static SDL_Window *win;
static SDL_Renderer *rend;
static SDL_Surface *game_over_surface, *score_surface;
static SDL_Texture *game_over_texture, *score_texture;
static SDL_Rect game_over_rect, score_rect;

static SDL_Color White = {0xff, 0xff, 0xff};
static SDL_Color Gray = {0xcc, 0xcc, 0xcc};
static TTF_Font *Font_18;
static TTF_Font *Font_32;

static int init_fonts() {
  if (TTF_Init() != 0) {
    printf("error initializing TTF: %s\\n", TTF_GetError());
    return -1;
  };

  Font_18 = TTF_OpenFont(FONT_PATH, 18);
  if (!Font_18) {
    printf("error opening font 18 %s\n%s\\n", FONT_PATH, TTF_GetError());
    TTF_Quit();
    return -1;
  }

  Font_32 = TTF_OpenFont(FONT_PATH, 32);
  if (!Font_32) {
    printf("error opening font 32 %s\n%s\\n", FONT_PATH, TTF_GetError());
    TTF_CloseFont(Font_18);
    TTF_Quit();
    return -1;
  }

  return 0;
}

int init_graphics() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("error initializing SDL: %s\\n", SDL_GetError());
    return -1;
  }

  win = SDL_CreateWindow(WIN_TITLE, SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, 0);

  if (!win) {
    printf("error creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return -1;
  }

  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  rend = SDL_CreateRenderer(win, -1, render_flags);
  if (!rend) {
    printf("error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return -1;
  }

  SDL_CreateRGBSurface(0, WIN_WIDTH, WIN_HEIGHT, 32, 0, 0, 0, 0);

  if (init_fonts() != 0) {
    SDL_DestroyWindow(win);
    SDL_Quit();
    return -1;
  };

  return 0;
}

static void render_score(int score) {
  char score_str[SCORE_SIZE];
  snprintf(score_str, SCORE_SIZE, "%i", score);

  score_surface = TTF_RenderText_Solid(Font_32, score_str, Gray);
  score_texture = SDL_CreateTextureFromSurface(rend, score_surface);

  score_rect.x = (GRID_WIDTH + 3) * BLOCK_SIZE - score_surface->w / 2;
  score_rect.y = BLOCK_SIZE;
  score_rect.w = score_surface->w;
  score_rect.h = score_surface->h;

  SDL_RenderCopy(rend, score_texture, NULL, &score_rect);
}

void render_game_over_message() {
  game_over_surface = TTF_RenderText_Solid(Font_18, GAME_OVER_TEXT, White);
  game_over_texture = SDL_CreateTextureFromSurface(rend, game_over_surface);

  game_over_rect.x = (WIN_WIDTH - game_over_surface->w) / 2;
  game_over_rect.y = (WIN_HEIGHT - game_over_surface->h) / 2;
  game_over_rect.w = game_over_surface->w;
  game_over_rect.h = game_over_surface->h;

  SDL_RenderCopy(rend, game_over_texture, NULL, &game_over_rect);
  SDL_RenderPresent(rend);
}

void draw_block(int x, int y, bool empty) {
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

  TTF_CloseFont(Font_18);
  TTF_CloseFont(Font_32);
  TTF_Quit();

  SDL_Quit();
}
