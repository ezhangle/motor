#include <stdlib.h>
#include <lauxlib.h>
#include "filesystem.h"
#include "tools.h"
#include "../filesystem/filesystem.h"

  
static int l_filesystem_read(lua_State* state) {
  // TODO implement max length
  char const* filename = l_tools_tostring_or_err(state, 1);
  char* data = NULL;
  int len = filesystem_read(filename, &data);
  if(len < 0) {
    lua_pushstring(state, "could not read file");
    return lua_error(state);
  }

  lua_pushstring(state, data);
  free(data);
  lua_pushnumber(state, len);
  return 2;
}

static luaL_Reg const regFuncs[] = {
  {"read", l_filesystem_read},
  {NULL, NULL}
};

int l_filesystem_register(lua_State* state) {
  l_tools_register_module(state, "filesystem", regFuncs);
}
