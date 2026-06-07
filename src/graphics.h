#pragma once
#include "game.h"

int init_graphics();
void render_state(game_state_t *state);
void release_resources();
void log_error(const char *msg);
void delay();
