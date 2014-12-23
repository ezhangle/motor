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
  int s1type = lua_type(state, 1);

  l_graphics_Image* image = (l_graphics_Image*)lua_newuserdata(state, sizeof(l_graphics_Image));

  if(s1type == LUA_TSTRING) {
    graphics_Image_new_with_filename(&image->image, lua_tostring(state, 1));
  } else if(s1type == LUA_TUSERDATA) {
    lua_getmetatable(state, 1);
    if(!l_image_isImageData(state, 1)) {
      lua_pushstring(state, "expected ImageData");
      return lua_error(state);
    }
    image_ImageData * imageData = (image_ImageData*)lua_touserdata(state, 1);
    graphics_Image_new_with_ImageData(&image->image, imageData);
  } else {
    lua_pushstring(state, "expected ImageData");
    return lua_error(state);
  }

//  lua_rawgeti(state, LUA_REGISTRYINDEX, 
}

static luaL_Reg const regFuncs[] = {
  {"setBackgroundColor", l_graphics_setBackgroundColor},
  {"setColor",           l_graphics_setColor},
  {"clear",              l_graphics_clear},
  {"newImage",           l_graphics_newImage},
  {NULL, NULL}
};

int l_graphics_register(lua_State* state) {
  l_tools_register_module(state, "graphics", regFuncs);

  return 0;
}
