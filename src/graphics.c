#include "graphics.h"

#define WIN_TITLE "Tetris"

#ifdef __ENSCRIPTEN__
#define FONT_PATH "font.ttf"
#else
#define FONT_PATH "src/assets/font.ttf"
#endif

#define SCORE_SIZE 7
#define LEVEL_SIZE 3

const int WIN_WIDTH = (GRID_WIDTH + 5) * BLOCK_SIZE;
const int WIN_HEIGHT = (GRID_HEIGHT + 2) * BLOCK_SIZE;

static SDL_Window *win;
static SDL_Renderer *rend;

static SDL_Color White = {0xff, 0xff, 0xff};
static SDL_Color Gray = {0xcc, 0xcc, 0xcc};
static TTF_Font *Font_18;
static TTF_Font *Font_32;

static int init_fonts() {
  if (TTF_Init() != 0) {
    SDL_LogError(0, "error initializing TTF: %s\\n", TTF_GetError());
    return -1;
  };

  Font_18 = TTF_OpenFont(FONT_PATH, 18);
  if (!Font_18) {
    SDL_LogError(0, "error opening font 18 %s\n%s\\n", FONT_PATH,
                 TTF_GetError());
    TTF_Quit();
    return -1;
  }

  Font_32 = TTF_OpenFont(FONT_PATH, 32);
  if (!Font_32) {
    SDL_LogError(0, "error opening font 32 %s\n%s\\n", FONT_PATH,
                 TTF_GetError());
    TTF_CloseFont(Font_18);
    TTF_Quit();
    return -1;
  }

  return 0;
}

int init_graphics() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_LogError(0, "error initializing SDL: %s\\n", SDL_GetError());
    return -1;
  }

  win = SDL_CreateWindow(WIN_TITLE, SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, 0);

  if (!win) {
    SDL_LogError(0, "error creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return -1;
  }

  rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC);
  if (!rend) {
    SDL_LogError(0, "error creating renderer: %s\n", SDL_GetError());
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

static void render_right_text(const char *text, int y, TTF_Font *Font) {
  SDL_Surface *surface = TTF_RenderText_Solid(Font, text, Gray);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, surface);

  SDL_Rect rect;
  rect.x = (GRID_WIDTH + 3) * BLOCK_SIZE - surface->w / 2;
  rect.y = y;
  rect.w = surface->w;
  rect.h = surface->h;

  SDL_RenderCopy(rend, texture, NULL, &rect);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
};

static void render_score(int score, int level) {
  char score_str[SCORE_SIZE];
  snprintf(score_str, SCORE_SIZE, "%0*d", SCORE_SIZE - 1, score);

  render_right_text("SCORE", BLOCK_SIZE, Font_18);
  render_right_text(score_str, BLOCK_SIZE * 2, Font_32);

  char level_str[3];
  snprintf(level_str, 3, "%0*d", LEVEL_SIZE - 1, level);

  render_right_text("LEVEL", BLOCK_SIZE * 6, Font_18);
  render_right_text(level_str, BLOCK_SIZE * 7, Font_32);
}

static void render_game_over_text(const char *text, int y, TTF_Font *Font) {
  SDL_Surface *surface = TTF_RenderText_Solid(Font, text, White);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, surface);

  SDL_Rect rect;
  rect.x = (WIN_WIDTH - surface->w) / 2;
  rect.y = y;
  rect.w = surface->w;
  rect.h = surface->h;

  SDL_RenderCopy(rend, texture, NULL, &rect);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void render_game_over_message(int score) {
  char score_str[SCORE_SIZE];
  snprintf(score_str, SCORE_SIZE, "%i", score);

  render_game_over_text("GAME OVER", WIN_HEIGHT / 2 - BLOCK_SIZE * 3, Font_32);
  render_game_over_text("YOU SCORED:", WIN_HEIGHT / 2 - BLOCK_SIZE * 2,
                        Font_32);
  render_game_over_text(score_str, WIN_HEIGHT / 2, Font_32);
  render_game_over_text("Press any key to restart...",
                        WIN_HEIGHT / 2 + BLOCK_SIZE * 2, Font_18);
  SDL_RenderPresent(rend);
}

void draw_block(int x, int y, int color) {
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

  SDL_SetRenderDrawColor(rend, 0x0c, 0x0c, 0x0c, 0xff);
  SDL_RenderFillRect(rend, &outer);

  unsigned int r, g, b;

  // Shift bits and extract 8 least significant bits for each color;
  r = (color >> 16) & 0xFF;
  g = (color >> 8) & 0xFF;
  b = color & 0xFF;

  SDL_SetRenderDrawColor(rend, r, g, b, 0xff);
  SDL_RenderFillRect(rend, &inner);
}

void clear_screen() {
  SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
  SDL_RenderClear(rend);
}

void render_frame(int score, int level) {
  render_score(score, level);
  SDL_RenderPresent(rend);
}

void release_resources() {
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);

  TTF_CloseFont(Font_18);
  TTF_CloseFont(Font_32);
  TTF_Quit();

  SDL_Quit();
}
