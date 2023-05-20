#include "definitions.h"

SDL_Window *win;
SDL_Renderer *rend;
SDL_Surface *surfaceMessage;
SDL_Texture *Message;
SDL_Rect Message_rect;

int init_graphics();
void clear_screen();
void present_screen();
void release_resources();
void draw_block(int x, int y);
