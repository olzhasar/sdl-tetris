#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 40
#define GRID_WIDTH 10
#define GRID_HEIGHT 20
#define SCORE_SINGLE 1
#define SCORE_LINE 100

#define N_SHAPES 7

extern int game_over;
extern int score;
