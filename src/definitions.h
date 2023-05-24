#include "SDL.h"
#include "SDL_timer.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

#define BLOCK_SIZE 40
#define GRID_WIDTH 10
#define GRID_HEIGHT 20
