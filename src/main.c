#include "definitions.h"
#include "game.h"

int main(int argc, char **argv) {
  if (init_game() != 0) {
    SDL_LogError(0, "Failed to start game\n");
    return 1;
  };

  if (game_loop() != 0) {
    SDL_LogError(0, "Unexpected error occured\n");
  };

  if (terminate_game() != 0) {
    SDL_LogError(0, "Error while terminating game\n");
  };

  return 0;
}
