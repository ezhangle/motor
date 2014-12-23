#include <lauxlib.h>
#include "image.h"
#include "tools.h"
#include "../image/imagedata.h"

static struct {
  int imageDataMT;
} moduleData;


int l_image_newImageData(lua_State* state) {
  image_ImageData* imageData = (image_ImageData*)lua_newuserdata(state, sizeof(image_ImageData));
  int s1type = lua_type(state, 1);
  if(s1type == LUA_TSTRING) {
    image_ImageData_new_with_filename(imageData, lua_tostring(state, 1));
  } else if(s1type == LUA_TNUMBER && lua_type(state, 2) == LUA_TNUMBER) {
    image_ImageData_new_with_size(imageData, lua_tointeger(state, 1), lua_tointeger(state, 2));
  } else {
    lua_pushstring(state, "need filename or size for imagedata");
    return lua_error(state);
  }

  lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.imageDataMT);
  lua_setmetatable(state, -2);

  return 1;
}

static int l_ImageData_gc(lua_State* state) {
  image_ImageData* imagedata = (image_ImageData*)lua_touserdata(state, 1);
  image_ImageData_free(imagedata);
  return 0;
}

static luaL_Reg const regFuncs[] = {
  {"newImageData", l_image_newImageData},
  {NULL, NULL}
};

bool l_image_isImageData(lua_State* state, int index) {
  if(lua_type(state, index) != LUA_TUSERDATA) {
    return false;
  }

  lua_getmetatable(state, index);
  lua_pushstring(state, "type");
  lua_rawget(state, -2);
  bool matching = lua_tointeger(state, -1);
  lua_pop(state, 2);
  return matching;
}

int l_image_register(lua_State* state) {
  l_tools_register_module(state, "image", regFuncs);

  lua_newtable(state);
  lua_pushstring(state, "__gc");
  lua_pushcfunction(state, l_ImageData_gc);
  lua_rawset(state, -3);
  lua_pushvalue(state, -1);
  moduleData.imageDataMT = luaL_ref(state, LUA_REGISTRYINDEX);
  lua_pushstring(state, "type");
  lua_pushinteger(state, moduleData.imageDataMT);
  lua_rawset(state, -3);

  lua_pop(state, 1);
  
  return 0;
}
