#pragma once

#include <lua.h>
#include <lauxlib.h>
#include <stdbool.h>

void l_tools_register_module(lua_State* state, char const* moduleName, luaL_Reg const * funcs);


#define l_check_type_fn(name, typeMT) \
bool name(lua_State* state, int index) { \
  if(lua_type(state, index) != LUA_TUSERDATA) \
    return 0;                                 \
  lua_getmetatable(state, index);             \
  lua_pushstring(state, "type");              \
  lua_rawget(state, -2);                      \
  bool matching = lua_tointeger(state, -1) == typeMT; \
  lua_pop(state, 2);                          \
  return matching;                          \
}

#define l_to_type_fn(name, type) \
type* name(lua_State* state, int index) {\
  return (type*)lua_touserdata(state, index);\
}
