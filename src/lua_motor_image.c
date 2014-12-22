#include <lauxlib.h>
#include "lua_motor_image.h"
#include "lua_tools.h"
#include "image_imagedata.h"

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

  return 1;
}

static luaL_Reg const regFuncs[] = {
  {"newImageData", motor_lua_motor_image_newImageData},
  {NULL, NULL}
};

int motor_lua_motor_image_register(lua_State* state) {
  motor_lua_tools_register_module(state, "image", regFuncs);
  
  return 0;
}
