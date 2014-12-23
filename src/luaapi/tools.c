#include "tools.h"

void l_tools_register_module(lua_State* state, char const* moduleName, luaL_Reg const * funcs) {
  lua_getglobal(state, "motor");
  lua_pushstring(state, moduleName);
  luaL_newlib(state, funcs);
  lua_rawset(state, -3);
  lua_pop(state, 1);

}
