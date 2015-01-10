#include <lauxlib.h>
#include "tools.h"
#include "graphics_canvas.h"

static struct {
  int canvasMT;
} moduleData;

l_check_type_fn(l_graphics_isCanvas, moduleData.canvasMT)
l_to_type_fn(l_graphics_toCanvas, graphics_Canvas)


int l_graphics_newCanvas(lua_State* state) {
  // TODO support default parameters

  int width = l_tools_tonumber_or_err(state, 1);
  int height = l_tools_tonumber_or_err(state, 2);
  
  graphics_Canvas *canvas = lua_newuserdata(state, sizeof(graphics_Canvas));
  graphics_Canvas_new(canvas, width, height);

  lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.canvasMT);
  lua_setmetatable(state, -2);

  return 1;
}

static int l_graphics_gcCanvas(lua_State* state) {
  if(!l_graphics_isCanvas(state, 1)) {
    lua_pushstring(state, "expected canvas");
    return lua_error(state);
  }

  graphics_Canvas *canvas = l_graphics_toCanvas(state, 1);

  graphics_Canvas_free(canvas);

  return 1;
}

static luaL_Reg const canvasMetatableFuncs[] = {
  {"__gc",               l_graphics_gcCanvas},
  {NULL, NULL}
};

void l_graphics_canvas_register(lua_State * state) {
  moduleData.canvasMT = l_tools_make_type_mt(state, canvasMetatableFuncs);
}
