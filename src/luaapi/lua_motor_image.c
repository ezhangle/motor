#include <lauxlib.h>
#include "lua_motor_image.h"
#include "lua_tools.h"
#include "../image/imagedata.h"

static struct {
  int imageDataMetatable;
} moduleData ;

static int motor_lua_motor_image_newImageData(lua_State* state) {
  motor_image_ImageData* imageData = (motor_image_ImageData*)lua_newuserdata(state, sizeof(motor_image_ImageData));
  int s1type = lua_type(state, 1);
  if(s1type == LUA_TSTRING) {
    motor_image_imagedata_new_with_filename(imageData, lua_tostring(state, 1));
  } else if(s1type == LUA_TNUMBER && lua_type(state, 2) == LUA_TNUMBER) {
    motor_image_imagedata_new_with_size(imageData, lua_tointeger(state, 1), lua_tointeger(state, 2));
  } else {
    lua_pushstring(state, "need filename or size for imagedata");
    return lua_error(state);
  }

  lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.imageDataMetatable);
  lua_setmetatable(state, -2);

  return 1;
}

static int motor_lua_motor_image_gc(lua_State* state) {
  motor_image_ImageData* imagedata = (motor_image_ImageData*)lua_touserdata(state, 1);
  motor_image_imagedata_free(imagedata);
  return 0;
}

static luaL_Reg const regFuncs[] = {
  {"newImageData", motor_lua_motor_image_newImageData},
  {NULL, NULL}
};

int motor_lua_motor_image_register(lua_State* state) {
  motor_lua_tools_register_module(state, "image", regFuncs);

  lua_newtable(state);
  lua_pushstring(state, "__gc");
  lua_pushcfunction(state, motor_lua_motor_image_gc);
  lua_rawset(state, -3);
  moduleData.imageDataMetatable = luaL_ref(state, LUA_REGISTRYINDEX);
  
  
  return 0;
}
