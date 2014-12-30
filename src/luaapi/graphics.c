#include <lauxlib.h>
#include "graphics.h"
#include "tools.h"
#include "../graphics/graphics.h"
#include "../graphics/matrixstack.h"
#include "../graphics/image.h"
#include "../graphics/quad.h"
#include "image.h"

static struct {
  int imageMT;
  int quadMT;
} moduleData;

static int l_graphics_setBackgroundColor(lua_State* state) {
  int red   = lua_tointeger(state, 1);
  int green = lua_tointeger(state, 2);
  int blue  = lua_tointeger(state, 3);
  int alpha = lua_tointeger(state, 4);

  graphics_setBackgroundColor(red, green, blue, alpha);
  return 0;
}

static int l_graphics_setColor(lua_State* state) {
  int red   = lua_tointeger(state, 1);
  int green = lua_tointeger(state, 2);
  int blue  = lua_tointeger(state, 3);
  int alpha = lua_tointeger(state, 4);

  graphics_setColor(red, green, blue, alpha);
  return 0;
}

static int l_graphics_clear(lua_State* state) {
  graphics_clear();
  return 0;
}

typedef struct {
  graphics_Image image;
  int imageDataRef;
} l_graphics_Image;

static int l_graphics_newImage(lua_State* state) {
  if(lua_type(state, 1) == LUA_TSTRING) {
    l_image_newImageData(state);
    lua_remove(state, 1);
  } 
  
  if(!l_image_isImageData(state, 1)) {
    lua_pushstring(state, "expected ImageData");
    return lua_error(state);
  }

  image_ImageData * imageData = (image_ImageData*)lua_touserdata(state, 1);
  int ref = luaL_ref(state, LUA_REGISTRYINDEX);

  l_graphics_Image *image = (l_graphics_Image*)lua_newuserdata(state, sizeof(l_graphics_Image));

  graphics_Image_new_with_ImageData(&image->image, imageData);
  image->imageDataRef = ref;
  printf("ref: %d\n", image->imageDataRef);

  lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.imageMT);
  lua_setmetatable(state, -2);

  return 1;
}


l_check_type_fn(l_graphics_isImage, moduleData.imageMT)
l_to_type_fn(l_graphics_toImage, l_graphics_Image)
l_check_type_fn(l_graphics_isQuad, moduleData.quadMT)
l_to_type_fn(l_graphics_toQuad, graphics_Quad)


static int l_graphics_gcImage(lua_State* state) {
  if(!l_graphics_isImage(state, 1)) {
    lua_pushstring(state, "Expected Image");
    return lua_error(state);
  }

  l_graphics_Image* img = l_graphics_toImage(state, 1);

  graphics_Image_free(&img->image);
  printf("ref: %d\n", img->imageDataRef);
  luaL_unref(state, LUA_REGISTRYINDEX, img->imageDataRef);
  return 0;
}


static int l_graphics_draw(lua_State* state) {
  if(!l_graphics_isImage(state, 1)) {
    lua_pushstring(state, "Expected Image");
    return lua_error(state);
  }

  graphics_draw_Image(&l_graphics_toImage(state, 1)->image);
  return 0;
}

static int l_graphics_push(lua_State* state) {
  if(matrixstack_push()) {
    lua_pushstring(state, "Matrix stack overflow");
    return lua_error(state);
  }
  return 0;
}

static int l_graphics_pop(lua_State* state) {
  if(matrixstack_pop()) {
    lua_pushstring(state, "Matrix stack underrun");
    return lua_error(state);
  }
  return 0;
}

static int l_graphics_translate(lua_State* state) {
  float x = l_tools_tonumber_or_err(state, 1);
  float y = l_tools_tonumber_or_err(state, 2);

  matrixstack_translate(x, y);
  return 0;
}

static int l_graphics_scale(lua_State* state) {
  float x = l_tools_tonumber_or_err(state, 1);
  float y = l_tools_tonumber_or_err(state, 2);

  matrixstack_scale(x, y);
  return 0;
}

static int l_graphics_origin(lua_State* state) {
  matrixstack_origin();
  return 0;
}

static int l_graphics_shear(lua_State* state) {
  lua_pushstring(state, "not implemented");
  lua_error(state);
  return 0;
}

static int l_graphics_rotate(lua_State* state) {
  float a = l_tools_tonumber_or_err(state, 1);

  matrixstack_rotate(a);
  return 0;
}

static int l_graphics_newQuad(lua_State* state) {
  float x = l_tools_tonumber_or_err(state, 1);
  float y = l_tools_tonumber_or_err(state, 2);
  float w = l_tools_tonumber_or_err(state, 3);
  float h = l_tools_tonumber_or_err(state, 4);
  float rw = l_tools_tonumber_or_err(state, 5);
  float rh = l_tools_tonumber_or_err(state, 6);

  graphics_Quad* obj = lua_newuserdata(state, sizeof(graphics_Quad));

  graphics_Quad_new_with_ref(obj, x, y, w, h, rw, rh);

  lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.quadMT);
  lua_setmetatable(state, -2);

  return 1;
}

static int l_graphics_Image_getDimensions(lua_State* state) {
  if(!l_graphics_isImage(state, 1)) {
    lua_pushstring(state, "expected image");
    return lua_error(state);
  }

  l_graphics_Image* img = l_graphics_toImage(state, 1);
  lua_pushinteger(state, img->image.width);
  lua_pushinteger(state, img->image.height);
  return 2;
}

static int l_graphics_Image_getWidth(lua_State* state) {
  if(!l_graphics_isImage(state, 1)) {
    lua_pushstring(state, "expected image");
    return lua_error(state);
  }

  l_graphics_Image* img = l_graphics_toImage(state, 1);
  lua_pushinteger(state, img->image.width);
  return 1;
}

static int l_graphics_Image_getHeight(lua_State* state) {
  if(!l_graphics_isImage(state, 1)) {
    lua_pushstring(state, "expected image");
    return lua_error(state);
  }

  l_graphics_Image* img = l_graphics_toImage(state, 1);
  lua_pushinteger(state, img->image.height);
  return 1;
}

static int l_graphics_Quad_getViewport(lua_State* state) {
  if(!l_graphics_isQuad(state, 1)) {
    lua_pushstring(state, "expected quad");
    return lua_error(state);
  }

  graphics_Quad *quad = l_graphics_toQuad(state, 1);
  lua_pushnumber(state, quad->x);
  lua_pushnumber(state, quad->y);
  lua_pushnumber(state, quad->w);
  lua_pushnumber(state, quad->h);

  return 4;
}

static int l_graphics_Quad_setViewport(lua_State* state) {
  if(!l_graphics_isQuad(state, 1)) {
    lua_pushstring(state, "expected quad");
    return lua_error(state);
  }

  graphics_Quad *quad = l_graphics_toQuad(state, 1);
  
  float x = l_tools_tonumber_or_err(state, 2);
  float y = l_tools_tonumber_or_err(state, 3);
  float w = l_tools_tonumber_or_err(state, 4);
  float h = l_tools_tonumber_or_err(state, 5);

  quad->x = x;
  quad->y = y;
  quad->w = w;
  quad->h = h;

  return 0;
}

static graphics_WrapMode l_graphics_Image_setFilter_getFilterMode(lua_State* state, int index) {
  char const* modestr = l_tools_tostring_or_err(state, index);
  graphics_FilterMode mode;
  if(!strcmp(modestr, "linear")) {
    mode = graphics_FilterMode_linear;
  } else if(!strcmp(modestr, "nearest")) {
    mode = graphics_FilterMode_nearest;
  } else {
    lua_pushstring(state, "invalid filter mode");
    lua_error(state);
  }

  return mode;
}

static int l_graphics_Image_setFilter(lua_State* state) {
  if(!l_graphics_isImage(state, 1)) {
    lua_pushstring(state, "expected image");
    return lua_error(state);
  }

  l_graphics_Image* img = l_graphics_toImage(state, 1);
  graphics_FilterMode minMode = l_graphics_Image_setFilter_getFilterMode(state, 2);
  graphics_FilterMode magMode = l_graphics_Image_setFilter_getFilterMode(state, 3);

  graphics_Image_setFilter(img, minMode, magMode);

  return 0;
}

static graphics_WrapMode l_graphics_Image_setWrap_getWrapMode(lua_State* state, int index) {
  char const* modestr = l_tools_tostring_or_err(state, index);
  graphics_WrapMode mode;
  if(!strcmp(modestr, "repeat")) {
    mode = graphics_WrapMode_repeat;
  } else if(!strcmp(modestr, "clamp")) {
    mode = graphics_WrapMode_clamp;
  } else {
    lua_pushstring(state, "invalid wrap mode");
    lua_error(state);
  }

  return mode;
}

static int l_graphics_Image_setWrap(lua_State* state) {
  if(!l_graphics_isImage(state, 1)) {
    lua_pushstring(state, "expected image");
    return lua_error(state);
  }

  l_graphics_Image* img = l_graphics_toImage(state, 1);
  graphics_WrapMode horMode = l_graphics_Image_setWrap_getWrapMode(state, 2);
  graphics_WrapMode verMode = l_graphics_Image_setWrap_getWrapMode(state, 3);

  graphics_Image_setFilter(img, horMode, verMode);

  return 0;
}

static void l_graphics_Image_getWrap_pushFilterMode(lua_State* state, graphics_FilterMode mode) {
  switch(mode) {
  case graphics_FilterMode_nearest:
    lua_pushstring(state, "nearest");
    break;
  case graphics_FilterMode_linear:
    lua_pushstring(state, "linear");
    break;
  }
}

static int l_graphics_Image_getFilter(lua_State* state) {
  if(!l_graphics_isImage(state, 1)) {
    lua_pushstring(state, "expected image");
    return lua_error(state);
  }

  l_graphics_Image* img = l_graphics_toImage(state, 1);

  graphics_FilterMode minMode, magMode;
  graphics_Image_getFilter(img, &minMode, &magMode);

  l_graphics_Image_getWrap_pushFilterMode(state, minMode);
  l_graphics_Image_getWrap_pushFilterMode(state, magMode);

  return 2;
}

static void l_graphics_Image_getWrap_pushWrapMode(lua_State* state, graphics_FilterMode mode) {
  switch(mode) {
  case graphics_WrapMode_clamp:
    lua_pushstring(state, "clamp");
    break;
  case graphics_WrapMode_repeat:
    lua_pushstring(state, "repeat");
    break;
  }
}

static int l_graphics_Image_getWrap(lua_State* state) {
  if(!l_graphics_isImage(state, 1)) {
    lua_pushstring(state, "expected image");
    return lua_error(state);
  }

  l_graphics_Image* img = l_graphics_toImage(state, 1);

  graphics_WrapMode modeHor, modeVert;
  graphics_Image_getFilter(img, &modeHor, &modeVert);

  l_graphics_Image_getWrap_pushWrapMode(state, modeHor);
  l_graphics_Image_getWrap_pushWrapMode(state, modeVert);


  return 2;
}

static luaL_Reg const regFuncs[] = {
  {"setBackgroundColor", l_graphics_setBackgroundColor},
  {"setColor",           l_graphics_setColor},
  {"clear",              l_graphics_clear},
  {"newImage",           l_graphics_newImage},
  {"newQuad",            l_graphics_newQuad},
  {"draw",               l_graphics_draw},
  {"push",               l_graphics_push},
  {"pop",                l_graphics_pop},
  {"origin",             l_graphics_origin},
  {"rotate",             l_graphics_rotate},
  {"scale",              l_graphics_scale},
  {"shear",              l_graphics_shear},
  {"translate",          l_graphics_translate},
  {NULL, NULL}
};

static luaL_Reg const imageMetatableFuncs[] = {
  {"__gc",               l_graphics_gcImage},
  {"getDimensions",      l_graphics_Image_getDimensions},
  {"getWidth",           l_graphics_Image_getWidth},
  {"getHeight",          l_graphics_Image_getHeight},
  {"setFilter",          l_graphics_Image_setFilter},
  {"getFilter",          l_graphics_Image_getFilter},
  {"setWrap",            l_graphics_Image_setWrap},
  {"getWrap",            l_graphics_Image_getWrap},
  {NULL, NULL}
};

static luaL_Reg const quadMetatableFuncs[] = {
  {"getViewport",        l_graphics_Quad_getViewport},
  {"setViewport",        l_graphics_Quad_setViewport},
  {NULL, NULL}
};

int l_graphics_register(lua_State* state) {
  l_tools_register_module(state, "graphics", regFuncs);

  moduleData.imageMT = l_tools_make_type_mt(state, imageMetatableFuncs);
  moduleData.quadMT  = l_tools_make_type_mt(state, quadMetatableFuncs);

  return 0;
}
