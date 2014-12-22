#include <lauxlib.h>
#include "lua_motor_graphics.h"
#include "lua_tools.h"
#include "../graphics/graphics.h"

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

static luaL_Reg const regFuncs[] = {
  {"setBackgroundColor", motor_lua_motor_graphics_setBackgroundColor},
  {"setColor",           motor_lua_motor_graphics_setColor},
  {"clear",              motor_lua_motor_graphics_clear},
  {NULL, NULL}
};

int motor_lua_motor_graphics_register(lua_State* state) {
  motor_lua_tools_register_module(state, "graphics", regFuncs);

  return 0;
}
