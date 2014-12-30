#include "tools.h"

void l_tools_register_module(lua_State* state, char const* moduleName, luaL_Reg const * funcs) {
  lua_getglobal(state, "motor");
  lua_pushstring(state, moduleName);
  luaL_newlib(state, funcs);
  lua_rawset(state, -3);
  lua_pop(state, 1);
}

float l_tools_tonumber_or_err(lua_State* state, int index) {
  if(lua_type(state, index) != LUA_TNUMBER) {
    lua_pushstring(state, "expected number");
    lua_error(state);
  }

  return lua_tonumber(state, index);
}

char const* l_tools_tostring_or_err(lua_State* state, int index) {
  if(lua_type(state, index) != LUA_TSTRING) {
    lua_pushstring(state, "expected string");
    lua_error(state);
  }

  return lua_tostring(state, index);
}

int l_tools_make_type_mt(lua_State* state, luaL_Reg const* funcs) {
  int mtref;
  luaL_newlib(state, funcs);
  lua_pushvalue(state, -1);
  mtref = luaL_ref(state, LUA_REGISTRYINDEX);
  lua_pushstring(state, "type");
  lua_pushinteger(state, mtref);
  lua_rawset(state, -3);
  lua_pushstring(state, "__index");
  lua_pushvalue(state, -2);
  lua_rawset(state, -3);

  lua_pop(state, 1);

  return mtref;
}
