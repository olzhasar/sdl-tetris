#include "game.h"
#include <SDL_keycode.h>

int close_requested = 0;

void handle_key_down(SDL_Keycode key_code) {
  switch (key_code) {
  case SDLK_LEFT:
  case SDLK_a:
    current_action = MOVE_LEFT;
    break;
  case SDLK_RIGHT:
  case SDLK_d:
    current_action = MOVE_RIGHT;
    break;
  case SDLK_UP:
  case SDLK_w:
    current_action = ROTATE;
    break;
  case SDLK_DOWN:
  case SDLK_s:
  case SDLK_SPACE:
    current_action = FALL;
  }
}

void listen_for_input() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      close_requested = 1;
    }
    if (event.type == SDL_KEYDOWN) {
      SDL_Keycode key_code = event.key.keysym.sym;

      if (key_code == SDLK_ESCAPE) {
        close_requested = 1;
      } else {
        if (!game_over) {
          handle_key_down(key_code);
        } else {
          restart_game();
        }
      }
    }
  }
}
