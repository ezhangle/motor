#include <lauxlib.h>
#include "lua_motor_graphics.h"
#include "lua_tools.h"
#include "../graphics/graphics.h"
#include "../graphics/image.h"
#include "lua_motor_image.h"

static struct {
  int imageMT;
} moduleData;

static int motor_lua_motor_graphics_setBackgroundColor(lua_State* state) {
  int red   = lua_tointeger(state, 1);
  int green = lua_tointeger(state, 2);
  int blue  = lua_tointeger(state, 3);
  int alpha = lua_tointeger(state, 4);

  motor_graphics_setBackgroundColor(red, green, blue, alpha);
  return 0;
}

static int motor_lua_motor_graphics_setColor(lua_State* state) {
  int red   = lua_tointeger(state, 1);
  int green = lua_tointeger(state, 2);
  int blue  = lua_tointeger(state, 3);
  int alpha = lua_tointeger(state, 4);

  motor_graphics_setColor(red, green, blue, alpha);
  return 0;
}

static int motor_lua_motor_graphics_clear(lua_State* state) {
  motor_graphics_clear();
  return 0;
}

typedef struct {
  motor_graphics_Image image;
  int imageDataRef;
} motor_lua_motor_graphics_Image;

static int motor_lua_motor_graphics_newImage(lua_State* state) {
  int s1type = lua_type(state, 1);

  motor_lua_motor_graphics_Image* image = (motor_lua_motor_graphics_Image*)lua_newuserdata(state, sizeof(motor_lua_motor_graphics_Image));

  if(s1type == LUA_TSTRING) {
    motor_graphics_image_new_with_filename(&image->image, lua_tostring(state, 1));
  } else if(s1type == LUA_TUSERDATA) {
    lua_getmetatable(state, 1);
    if(!motor_lua_motor_image_is_ImageData(state, 1)) {
      lua_pushstring(state, "expected ImageData");
      return lua_error(state);
    }
    motor_image_ImageData * imageData = (motor_image_ImageData*)lua_touserdata(state, 1);
    motor_graphics_image_new_with_imagedata(&image->image, imageData);
  } else {
    lua_pushstring(state, "expected ImageData");
    return lua_error(state);
  }

  lua_rawgeti(state, LUA_REGISTRYINDEX, 
}

static luaL_Reg const regFuncs[] = {
  {"setBackgroundColor", motor_lua_motor_graphics_setBackgroundColor},
  {"setColor",           motor_lua_motor_graphics_setColor},
  {"clear",              motor_lua_motor_graphics_clear},
  {"newImage",           motor_lua_motor_graphics_newImage},
  {NULL, NULL}
};

int motor_lua_motor_graphics_register(lua_State* state) {
  motor_lua_tools_register_module(state, "graphics", regFuncs);

  return 0;
}
