#include <lauxlib.h>
#include "tools.h"
#include "graphics_canvas.h"
#include "../graphics/graphics.h"
#include "graphics.h"

static struct {
  int canvasMT;
  graphics_Canvas* currentCanvas;
  int currentCanvasRef;
} moduleData;

l_checkTypeFn(l_graphics_isCanvas, moduleData.canvasMT)
l_toTypeFn(l_graphics_toCanvas, graphics_Canvas)


int l_graphics_newCanvas(lua_State* state) {
  // TODO support default parameters

  int width = l_tools_toNumberOrError(state, 1);
  int height = l_tools_toNumberOrError(state, 2);
  
  graphics_Canvas *canvas = lua_newuserdata(state, sizeof(graphics_Canvas));
  graphics_Canvas_new(canvas, width, height);

  lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.canvasMT);
  lua_setmetatable(state, -2);

  return 1;
}

static int l_graphics_gcCanvas(lua_State* state) {
  l_assertType(state, 1, l_graphics_isCanvas);

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

static int l_graphics_Canvas_renderTo(lua_State* state) {
  l_assertType(state, 1, l_graphics_isCanvas);
  l_assertType(state, 2, lua_isfunction);


  graphics_Canvas *canvas = l_graphics_toCanvas(state, 1);
  graphics_Canvas const *oldCanvas = graphics_getCanvas();
  graphics_setCanvas(canvas);
  lua_call(state, 0, 0);
  graphics_setCanvas(oldCanvas);
  return 0;
}

static int l_graphics_Canvas_getDimensions(lua_State* state) {
  l_assertType(state, 1, l_graphics_isCanvas);

  graphics_Canvas* img = l_graphics_toCanvas(state, 1);
  lua_pushinteger(state, img->image.width);
  lua_pushinteger(state, img->image.height);
  return 2;
}

static int l_graphics_Canvas_getWidth(lua_State* state) {
  l_assertType(state, 1, l_graphics_isCanvas);

  graphics_Canvas* img = l_graphics_toCanvas(state, 1);
  lua_pushinteger(state, img->image.width);
  return 1;
}

static int l_graphics_Canvas_getHeight(lua_State* state) {
  l_assertType(state, 1, l_graphics_isCanvas);

  graphics_Canvas* img = l_graphics_toCanvas(state, 1);
  lua_pushinteger(state, img->image.height);
  return 1;
}

static int l_graphics_Canvas_getWrap(lua_State* state) {
  l_assertType(state, 1, l_graphics_isCanvas);

  graphics_Canvas* img = l_graphics_toCanvas(state, 1);

  graphics_Wrap wrap;
  graphics_Image_getWrap(&img->image, &wrap);

  l_tools_pushEnum(state, wrap.horMode, l_graphics_WrapMode);
  l_tools_pushEnum(state, wrap.verMode, l_graphics_WrapMode);

  return 2;
}

static int l_graphics_Canvas_setWrap(lua_State* state) {
  l_assertType(state, 1, l_graphics_isCanvas);

  graphics_Canvas* img = l_graphics_toCanvas(state, 1);
  graphics_Wrap wrap;
  wrap.horMode = l_tools_toEnumOrError(state, 2, l_graphics_WrapMode);
  wrap.verMode = l_tools_toEnumOrError(state, 3, l_graphics_WrapMode);

  graphics_Image_setWrap(&img->image, &wrap);

  return 0;
}

static int l_graphics_Canvas_getFilter(lua_State* state) {
  l_assertType(state, 1, l_graphics_isCanvas);

  graphics_Canvas* img = l_graphics_toCanvas(state, 1);

  graphics_Filter filter;

  graphics_Image_getFilter(&img->image, &filter);

  l_tools_pushEnum(state, filter.minMode, l_graphics_FilterMode);
  l_tools_pushEnum(state, filter.magMode, l_graphics_FilterMode);
  lua_pushnumber(state, filter.maxAnisotropy);

  return 3;
}

static int l_graphics_Canvas_setFilter(lua_State* state) {
  l_assertType(state, 1, l_graphics_isCanvas);

  graphics_Canvas* img = l_graphics_toCanvas(state, 1);
  graphics_Filter newFilter;
  graphics_Image_getFilter(&img->image, &newFilter);
  newFilter.minMode = l_tools_toEnumOrError(state, 2, l_graphics_FilterMode);
  newFilter.magMode = l_tools_toEnumOrError(state, 3, l_graphics_FilterMode);
  newFilter.maxAnisotropy = luaL_optnumber(state, 4, 1.0f);
  graphics_Image_setFilter(&img->image, &newFilter);

  return 0;
}

static luaL_Reg const canvasMetatableFuncs[] = {
  {"__gc",               l_graphics_gcCanvas},
  {"renderTo",           l_graphics_Canvas_renderTo},
  {"getDimensions",      l_graphics_Canvas_getDimensions},
  {"getWidth",           l_graphics_Canvas_getWidth},
  {"getHeight",          l_graphics_Canvas_getHeight},
  {"setFilter",          l_graphics_Canvas_setFilter},
  {"getFilter",          l_graphics_Canvas_getFilter},
  {"setWrap",            l_graphics_Canvas_setWrap},
  {"getWrap",            l_graphics_Canvas_getWrap},
  //{"getData",            l_graphics_Image_getData},
  {NULL, NULL}
};

static luaL_Reg const canvasFreeFuncs[] = {
  {"newCanvas",          l_graphics_newCanvas},
  {"setCanvas",          l_graphics_setCanvas},
  {"getCanvas",          l_graphics_getCanvas},
  {NULL, NULL}
};

void l_graphics_canvas_register(lua_State * state) {
  moduleData.canvasMT = l_tools_makeTypeMetatable(state, canvasMetatableFuncs);
  l_tools_registerFuncsInModule(state, "graphics", canvasFreeFuncs);
}
