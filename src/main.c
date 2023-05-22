#include "game.h"

int main(void) {
  if (init_game() != 0) {
    printf("Failed to start game\n");
    return 1;
  };

  if (game_loop() != 0) {
    printf("Unexpected error occured\n");
  };

  if (terminate_game() != 0) {
    printf("Error while terminating game\n");
  };

  return 0;
}
