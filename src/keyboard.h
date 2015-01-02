#pragma once
#include <SDL/SDL_keyboard.h>

void keyboard_init();
char const * keyboard_getKeyName(SDL_Keycode key);
