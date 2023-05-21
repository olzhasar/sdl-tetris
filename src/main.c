#include "game.h"
#include "input.h"

int main(void) {
  init_game();

  while (!close_requested) {

    if (!game_over) {
      game_loop();
    }

    listen_for_input();
  }

  terminate_game();

  return 0;
}
