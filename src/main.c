#include "game.h"
#include "graphics.h"
#include "input.h"
#include <stdlib.h>
#include <time.h>

int game_loop(game_state_t *state) {
  input_event_t event = listen_for_input();
  if (event == QUIT) {
    return 1;
  }

  game_state_update(state, event);
  render_state(state);

  delay();

  return 0;
}
//
// #ifdef __EMSCRIPTEN__
// void run_loop() { game_loop(); }
// #endif

int main(int argc, char **argv) {
  // #ifndef __EMSCRIPTEN__
  srand(time(NULL)); // seed the random number generator
                     // #endif

  // #ifdef __EMSCRIPTEN__
  //   emscripten_set_main_loop(run_loop, 0, 1);
  // #else
  init_graphics();
  game_state_t state = game_state_new();

  while (1) {
    int res = game_loop(&state);

    if (res != 0) {
      if (res < 0) {
        log_error("Unexpected error occured\n");
      }
      break;
    }
  }
  // #endif

  release_resources();

  return 0;
}
