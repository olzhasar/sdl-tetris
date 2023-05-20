#include <SDL2/SDL.h>

#define BLOCK_SIZE 50

#define GRID_WIDTH 10
#define GRID_HEIGHT 20

#define N_SHAPES 5
#define SHAPE_SIZE 4
#define FRAME_DELAY 400

extern int game_over;

void GameLoop(SDL_Renderer *rend);
void PrepareForStart();
void RestartGame();

void HandleKeyDown(SDL_Event event);
