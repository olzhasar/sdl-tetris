#include "SDL_log.h"
#include "SDL_timer.h"
#include "game.h"
#include "graphics.h"
#include "input.h"
#include <stdlib.h>
#include <time.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static const int FRAME_DELAY = 16; // 1000 / 16 ~= 60fps
static game_state_t state;
static Uint64 prev_tick;

int game_loop(game_state_t *state) {
  Uint64 current_tick = SDL_GetTicks64();
  unsigned int elapsed_ms = (unsigned int)(current_tick - prev_tick);
  prev_tick = current_tick;

  input_event_t event = listen_for_input();
#ifndef __EMSCRIPTEN__
  if (event == QUIT) {
    return 1;
  }
#endif

  game_state_update(state, event, elapsed_ms);
  render_state(state);

#ifndef __EMSCRIPTEN__
  SDL_Delay(FRAME_DELAY);
#endif

  return 0;
}

#ifdef __EMSCRIPTEN__
static void run_loop(void) {
  if (game_loop(&state) != 0) {
    teardown_graphics();
    emscripten_cancel_main_loop();
  }
}
#endif

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  srand(time(NULL));

  if (init_graphics() != 0) {
    return EXIT_FAILURE;
  }

  state = game_state_new();
  prev_tick = SDL_GetTicks64();

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(run_loop, 0, 1);
#else
  while (1) {
    int res = game_loop(&state);

    if (res != 0) {
      if (res < 0) {
        SDL_LogError(0, "Unexpected error occured\n");
      }
      break;
    }
  }

  teardown_graphics();
#endif

  return 0;
}
