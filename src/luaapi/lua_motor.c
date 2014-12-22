#include "lua_motor.h"
#include "../motor.h"

int motor_lua_motor_getVersion(lua_State* state) {
  motor_Version const * version = motor_getVersion();
  lua_pushnumber(state, version->major);
  lua_pushnumber(state, version->minor);
  lua_pushnumber(state, version->revision);
  lua_pushstring(state, version->codename);
  return 4;
}

int motor_lua_motor_register(lua_State* state) {
  lua_newtable(state);

  lua_pushstring(state, "getVersion");
  lua_pushcfunction(state, motor_lua_motor_getVersion);
  lua_rawset(state, -3);
  
  lua_setglobal(state, "motor");

  return 0;
}
