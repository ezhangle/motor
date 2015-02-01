#pragma once

#include <lua.h>
#include <lauxlib.h>
#include <stdbool.h>
#include <string.h>

void l_tools_register_funcs_in_module(lua_State* state, char const* module, luaL_Reg const* funcs);
void l_tools_register_module(lua_State* state, char const* moduleName, luaL_Reg const * funcs);
//float l_tools_tonumber_or_err(lua_State* state, int index);
//char const* l_tools_tostring_or_err(lua_State* state, int index);
int l_tools_make_type_mt(lua_State* state, luaL_Reg const* funcs);


#ifndef MOTOR_SKIP_SAFETY_CHECKS
inline float l_tools_tonumber_or_err(lua_State* state, int index) {
  if(lua_type(state, index) != LUA_TNUMBER) {
    lua_pushstring(state, "expected number");
    lua_error(state);
  }

  return lua_tonumber(state, index);
}

inline char const* l_tools_tostring_or_err(lua_State* state, int index) {
  if(lua_type(state, index) != LUA_TSTRING) {
    lua_pushstring(state, "expected string");
    lua_error(state);
  }

  return lua_tostring(state, index);
}
#else
inline float l_tools_tonumber_or_err(lua_State* state, int index) {
  return lua_tonumber(state, index);
}

inline char const* l_tools_tostring_or_err(lua_State* state, int index) {
  return lua_tostring(state, index);
}
#endif

typedef struct {
  char const * name;
  int value;
} l_tools_Enum;

inline int l_tools_toenum_or_err(lua_State* state, int index, l_tools_Enum const* values) {
  char const* string = l_tools_tostring_or_err(state, index);

  while(values->name) {
    if(!strcmp(values->name, string)) {
      return values->value;
    }
    ++values;
  }

  lua_pushstring(state, "invalid enum value");
  return lua_error(state);
}

void l_tools_pushenum(lua_State* state, int value, l_tools_Enum const* values);

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

#ifndef MOTOR_SKIP_SAFETY_CHECKS
// TODO appropriate name
# define l_assert_type(state, index, func) \
    if(!func(state, index)) { \
     lua_pushstring(state, "expected X"); \
     lua_error(state); \
    }
#else
# define l_assert_type(state, index, func)
#endif
