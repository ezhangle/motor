#ifdef EMSCRIPTEN
#include <emscripten.h>
#else
#include <time.h>
#endif
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luaapi/graphics.h"
#include "luaapi/graphics_font.h"
#include "luaapi/image.h"
#include "luaapi/motor.h"
#include "luaapi/boot.h"
#include "luaapi/keyboard.h"
#include "luaapi/filesystem.h"
#include "luaapi/timer.h"

#include "graphics/graphics.h"

#include "keyboard.h"
#include "timer/timer.h"

double curtime() {
#ifdef EMSCRIPTEN
  return emscripten_get_now() / 1000.0;
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


int lua_errorhandler(lua_State *state) {
  const char* msg = lua_tostring(state, 1);
  //luaL_traceback(state, state, msg, 1);
  return 1;
}

typedef struct {
//  double lastTime;
  lua_State *luaState;
  int errhand;
} MainLoopData;

void main_loop(void *data) {
  MainLoopData* loopData = (MainLoopData*)data;

 /* double newTime = curtime();
  double deltaTime = newTime - loopData->lastTime;
  */

  timer_step();
  lua_rawgeti(loopData->luaState, LUA_REGISTRYINDEX, loopData->errhand);
  lua_getglobal(loopData->luaState, "motor");
  lua_pushstring(loopData->luaState, "update");

  // TODO use pcall, add error handling
  lua_rawget(loopData->luaState, -2);
  lua_pushnumber(loopData->luaState, timer_getDelta());
  if(lua_pcall(loopData->luaState, 1, 0, 1)) {
    printf("Lua error: %s\n", lua_tostring(loopData->luaState, -1));
    emscripten_force_exit(1);
  }

  lua_pushstring(loopData->luaState, "draw");
  lua_rawget(loopData->luaState, -2);

  // TODO use pcall, add error handling
  if(lua_pcall(loopData->luaState, 0, 0, 1)) {
    printf("Lua error: %s\n", lua_tostring(loopData->luaState, -1));
    emscripten_force_exit(1);
  }

  graphics_swap();

  lua_pop(loopData->luaState, 1);

  //loopData->lastTime = newTime;

  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_KEYDOWN:
      keyboard_keypressed(event.key.keysym.sym);
      break;
    case SDL_KEYUP:
      keyboard_keyreleased(event.key.keysym.sym);
      break;
    case SDL_TEXTINPUT:
      keyboard_textInput(event.text.text);
      break;

    }
  }
  //lua_gc(loopData->luaState, LUA_GCCOLLECT, 0);
}

int main()
{
  lua_State *lua = luaL_newstate();
  luaL_openlibs(lua);

  l_motor_register(lua);
  l_graphics_register(lua);
  l_image_register(lua);
  l_keyboard_register(lua);
  l_filesystem_register(lua);
  l_timer_register(lua);

  motor_Config config;

  l_boot(lua, &config);

  keyboard_init();
  graphics_init(config.window.width, config.window.height);

  if(luaL_dofile(lua, "/main.lua")) {
    printf("Error: %s\n", lua_tostring(lua, -1));
  }

  lua_getglobal(lua, "motor");
  lua_pushstring(lua, "load");
  lua_rawget(lua, -2);
  lua_call(lua, 0, 0);
  lua_pop(lua, 1);

  lua_pushcfunction(lua, lua_errorhandler);
  MainLoopData mainLoopData = {
//    .lastTime = curtime(),
    .luaState = lua,
    .errhand = luaL_ref(lua, LUA_REGISTRYINDEX)
  };

  timer_init();
  emscripten_set_main_loop_arg(main_loop, &mainLoopData, 0, 1);
}
