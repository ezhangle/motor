#include <lauxlib.h>
#include "tools.h"
#include "graphics_shader.h"
#include "../graphics/shader.h"

static struct {
  int shaderMT;
} moduleData;

int l_graphics_newShader(lua_State* state) {
  char const* str1 = l_tools_tostring_or_err(state, 1);
  char const* str2 = NULL;
  if(lua_isstring(state, 2)) {
    str2 = lua_tostring(state, 2);
  }

  graphics_Shader * shader = lua_newuserdata(state, sizeof(graphics_Shader));
  graphics_Shader_new(shader, str1, str2);

  return 1;
}

static luaL_Reg const shaderMetatableFuncs[] = {

};

void l_graphics_shader_register(lua_State *state) {
  moduleData.shaderMT  = l_tools_make_type_mt(state, shaderMetatableFuncs);
}
