#pragma once

#include <lua.h>
#include "../graphics/canvas.h"

void l_graphics_canvas_register(lua_State * state);
int l_graphics_newCanvas(lua_State* state);
bool l_graphics_isCanvas(lua_State* state, int index);
graphics_Canvas* l_graphics_toCanvas(lua_State* state, int index);
