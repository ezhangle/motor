#include <lauxlib.h>
#include "tools.h"
#include "graphics_canvas.h"
#include "../graphics/graphics.h"

static struct {
  int canvasMT;
  graphics_Canvas* currentCanvas;
  int currentCanvasRef;
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

static int l_graphics_setCanvas(lua_State* state) {
  graphics_Canvas *canvas = NULL;

  if(l_graphics_isCanvas(state, 1)) {
    canvas = l_graphics_toCanvas(state, 1);
  } else if(!lua_isnoneornil(state, 1)) {
    lua_pushstring(state, "expected none or canvas");
    return lua_error(state);
  }

  if(moduleData.currentCanvas) {
    moduleData.currentCanvas = NULL;
    luaL_unref(state, LUA_REGISTRYINDEX, moduleData.currentCanvasRef);
  }

  // TODO support multiple canvases
  lua_settop(state, 1);

  graphics_setCanvas(canvas);
  if(canvas) {
    moduleData.currentCanvas = canvas;
    moduleData.currentCanvasRef = luaL_ref(state, LUA_REGISTRYINDEX);
  }

  return 0;
}

static int l_graphics_getCanvas(lua_State* state) {
  if(moduleData.currentCanvas) {
    lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.currentCanvasRef);
    return 1;
 }
 lua_pushnil(state);
 return 1;
}

static luaL_Reg const canvasMetatableFuncs[] = {
  {"__gc",               l_graphics_gcCanvas},
  {NULL, NULL}
};

static luaL_Reg const canvasFreeFuncs[] = {
  {"newCanvas",          l_graphics_newCanvas},
  {"setCanvas",          l_graphics_setCanvas},
  {"getCanvas",          l_graphics_getCanvas},
  {NULL, NULL}
};

void l_graphics_canvas_register(lua_State * state) {
  moduleData.canvasMT = l_tools_make_type_mt(state, canvasMetatableFuncs);
  l_tools_register_funcs_in_module(state, "graphics", canvasFreeFuncs);
}
