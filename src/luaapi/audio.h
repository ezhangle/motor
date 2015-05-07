#pragma once

#include <lua.h>
#include <stdbool.h>
#include "../audio/audio.h"

int l_audio_register(lua_State *state);
bool l_audio_isSource(lua_State* state, int index);
audio_Source* l_audio_toSource(lua_State* state, int index);
