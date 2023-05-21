#include "definitions.h"
#include "graphics.h"

void game_loop();
void init_game();
void restart_game();
void end_game();
void terminate_game();

void handle_key_down(SDL_Event event);

enum Action { MOVE_LEFT, MOVE_RIGHT, MOVE_DOWN, ROTATE, FALL };
extern enum Action current_action;
