#include <SDL2/SDL.h>

#define BLOCK_SIZE 50
#define GRID_WIDTH 11
#define GRID_HEIGHT 15
#define FRAME_DELAY 50
#define VELOCITY 10

void GameLoop(SDL_Renderer *rend);

void HandleKeyDown(SDL_Event event);
