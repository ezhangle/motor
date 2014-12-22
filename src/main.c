#ifdef EMSCRIPTEN
#include <emscripten.h>
#else
#include <time.h>
#endif
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luaapi/lua_motor_graphics.h"
#include "luaapi/lua_motor_image.h"
#include "luaapi/lua_motor.h"
#include "luaapi/lua_boot.h"

#include "graphics/graphics.h"

double curtime() {
#ifdef EMSCRIPTEN
  return emscripten_get_now();
#else
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)(ts.tv_sec)*1000.0 + (double)(ts.tv_nsec) / 1000000.0;
#endif
}


int lua_curtime(lua_State *state) {
  lua_pushnumber(state, curtime());
  return 1;
}


typedef struct {
  double lastTime;
  lua_State *luaState;
} motor_MainLoopData;

void main_loop(void *data) {
  motor_MainLoopData* loopData = (motor_MainLoopData*)data;

  double newTime = curtime();
  double deltaTime = newTime - loopData->lastTime;

  lua_getglobal(loopData->luaState, "motor");
  lua_pushstring(loopData->luaState, "update");

  // TODO use pcall, add error handling
  lua_rawget(loopData->luaState, -2);
  lua_pushnumber(loopData->luaState, deltaTime);
  lua_call(loopData->luaState, 1, 0);

  lua_pushstring(loopData->luaState, "draw");
  lua_rawget(loopData->luaState, -2);

  // TODO use pcall, add error handling
  lua_call(loopData->luaState, 0, 0);
  motor_graphics_swap();

  lua_pop(loopData->luaState, 1);

  loopData->lastTime = newTime;

}

int main()
{
  lua_State *lua = luaL_newstate();
  luaL_openlibs(lua);


  motor_lua_motor_register(lua);
  motor_lua_motor_graphics_register(lua);
  motor_lua_motor_image_register(lua);

  lua_pushcfunction(lua, &lua_curtime);
  lua_setglobal(lua, "gettime");

  motor_Config config;

  motor_lua_boot(lua, &config);

  motor_graphics_init(config.window.width, config.window.height);

  if(luaL_dofile(lua, "/main.lua")) {
    printf("Error: %s\n", lua_tostring(lua, -1));
  }

  lua_getglobal(lua, "motor");
  lua_pushstring(lua, "load");
  lua_rawget(lua, -2);
  lua_call(lua, 0, 0);
  lua_pop(lua, 1);

  motor_MainLoopData mainLoopData = {
    .lastTime = curtime(),
    .luaState = lua
  };

  emscripten_set_main_loop_arg(main_loop, &mainLoopData, 0, 1);
}
