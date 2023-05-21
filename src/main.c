#include "main.h"

int main(void) {
  init_game();

  int close_requested = 0;

  while (!close_requested) {
    SDL_Event event;

    if (!game_over) {
      game_loop();
    }

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        close_requested = 1;
      }
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          close_requested = 1;
        } else {
          if (!game_over) {
            handle_key_down(event);
          } else {
            restart_game();
          }
        }
      }
    }
  }

  terminate_game();

  return 0;
}
