#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 50
#define GRID_WIDTH 10
#define GRID_HEIGHT 20

#define N_SHAPES 5
#define SHAPE_SIZE 4
#define FRAME_DELAY 400
#define FALL_DELAY 250

extern int game_over;
