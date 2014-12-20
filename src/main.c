#include <SDL.h>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#else
#include <time.h>
#endif
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "lua_motor.h"
#include "lua_boot.h"

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

  lua_pop(loopData->luaState, 1);

  loopData->lastTime = newTime;
}

int main()
{
  lua_State *lua = luaL_newstate();
  luaL_openlibs(lua);


  motor_lua_motor_register(lua);

  lua_pushcfunction(lua, &lua_curtime);
  lua_setglobal(lua, "gettime");

  motor_lua_boot(lua);

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  SDL_CreateWindowAndRenderer(600, 600, SDL_WINDOW_OPENGL, &window, &renderer);

  if(luaL_dofile(lua, "/main.lua")) {
    printf("Error: %s\n", lua_tostring(lua, -1));
  }

  motor_MainLoopData mainLoopData = {
    .lastTime = curtime(),
    .luaState = lua
  };

  emscripten_set_main_loop_arg(main_loop, &mainLoopData, 0, 1);
}
