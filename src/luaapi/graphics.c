#include <lauxlib.h>
#include "graphics.h"
#include "tools.h"
#include "../graphics/graphics.h"
#include "../graphics/image.h"
#include "image.h"

static struct {
  int imageMT;
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

static luaL_Reg const regFuncs[] = {
  {"setBackgroundColor", l_graphics_setBackgroundColor},
  {"setColor",           l_graphics_setColor},
  {"clear",              l_graphics_clear},
  {"newImage",           l_graphics_newImage},
  {"draw",               l_graphics_draw},
  {NULL, NULL}
};

static luaL_Reg const imageMetatableFuncs[] = {
  {"__gc",               l_graphics_gcImage},
  {NULL, NULL}
};

int l_graphics_register(lua_State* state) {
  l_tools_register_module(state, "graphics", regFuncs);

  luaL_newlib(state, imageMetatableFuncs);

  lua_pushvalue(state, -1);
  moduleData.imageMT = luaL_ref(state, LUA_REGISTRYINDEX);
  lua_pushstring(state, "type");
  lua_pushinteger(state, moduleData.imageMT);
  lua_rawset(state, -3);

  lua_pop(state, 1);

  return 0;
}
