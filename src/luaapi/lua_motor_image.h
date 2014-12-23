#pragma once

#include <lua.h>
#include <stdbool.h>

bool motor_lua_motor_image_is_ImageData(lua_State* state, int index);
int motor_lua_motor_image_register(lua_State* state);
