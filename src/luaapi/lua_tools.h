#pragma once

#include <lua.h>
#include <lauxlib.h>

void motor_lua_tools_register_module(lua_State* state, char const* moduleName, luaL_Reg const * funcs);
