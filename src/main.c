#ifdef EMSCRIPTEN
# include <emscripten.h>
#endif
#include <stdio.h>
#include <dirent.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luaapi/audio.h"
#include "luaapi/graphics.h"
#include "luaapi/graphics_font.h"
#include "luaapi/image.h"
#include "luaapi/motor.h"
#include "luaapi/boot.h"
#include "luaapi/keyboard.h"
#include "luaapi/mouse.h"
#include "luaapi/filesystem.h"
#include "luaapi/timer.h"
#include "luaapi/math.h"
#include "luaapi/event.h"

#include "graphics/graphics.h"

#include "audio/audio.h"

#include "keyboard.h"
#include "mouse.h"
#include "timer/timer.h"
#include "math/math.h"





int lua_errorhandler(lua_State *state) {
  // TODO
  //const char* msg = lua_tostring(state, 1);
  lua_Debug debug;
  int level = 0;
  while(lua_getstack(state, level, &debug)) {
    lua_getinfo(state, "Sl", &debug);
    lua_pushstring(state, debug.short_src);
    lua_pushstring(state, ":");
    lua_pushnumber(state, debug.currentline);
    lua_pushstring(state, "\n");
    ++level;
  }
  lua_concat(state, 4*level+1);
  return 1;
}

typedef struct {
//  double lastTime;
  lua_State *luaState;
  int errhand;
} MainLoopData;

void main_loop(void *data) {
  MainLoopData* loopData = (MainLoopData*)data;

  // TODO use registry to get love.update and love.draw?

 /* double newTime = curtime();
  double deltaTime = newTime - loopData->lastTime;
  */

  timer_step();
  lua_rawgeti(loopData->luaState, LUA_REGISTRYINDEX, loopData->errhand);
  lua_getglobal(loopData->luaState, "love");
  lua_pushstring(loopData->luaState, "update");

  // TODO use pcall, add error handling
  lua_rawget(loopData->luaState, -2);
  lua_pushnumber(loopData->luaState, timer_getDelta());
  //if(lua_pcall(loopData->luaState, 1, 0, 1)) {
  if(lua_pcall(loopData->luaState, 1, 0, 1)) {
    printf("Lua error: %s\n", lua_tostring(loopData->luaState, -1));
    #ifdef EMSCRIPTEN
      emscripten_force_exit(1);
    #else
      exit(1);
    #endif
  }

  lua_pushstring(loopData->luaState, "draw");
  lua_rawget(loopData->luaState, -2);

  // TODO use pcall, add error handling
  //if(lua_pcall(loopData->luaState, 0, 0, 1)) {
  if(lua_pcall(loopData->luaState, 0, 0, 1)) {
    printf("Lua error: %s\n", lua_tostring(loopData->luaState, -1));
    #ifdef EMSCRIPTEN
      emscripten_force_exit(1);
    #else
      exit(1);
    #endif
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
    case SDL_MOUSEMOTION:
      mouse_mousemoved(event.motion.x, event.motion.y);
      break;
    case SDL_MOUSEBUTTONDOWN:
      mouse_mousepressed(event.button.x, event.button.y,
      event.button.button);
      break;
    case SDL_MOUSEBUTTONUP:
      mouse_mousereleased(event.button.x, event.button.y,
      event.button.button);
      break;
#ifndef EMSCRIPTEN
    case SDL_QUIT:
      exit(0);
#endif
    }
  }

  audio_updateStreams();
  //lua_gc(loopData->luaState, LUA_GCCOLLECT, 0);
}

int main() {
  lua_State *lua = luaL_newstate();
  luaL_openlibs(lua);


  motor_Config config;
  l_motor_register(lua);
  l_audio_register(lua);
  l_graphics_register(lua);
  l_image_register(lua);
  l_keyboard_register(lua);
  l_mouse_register(lua);
  l_filesystem_register(lua);
  l_timer_register(lua);
  l_math_register(lua);
  l_event_register(lua);

  l_boot(lua, &config);

  keyboard_init();
  graphics_init(config.window.width, config.window.height);
  audio_init();
  math_init();


  if(luaL_dofile(lua, "main.lua")) {
    printf("Error: %s\n", lua_tostring(lua, -1));
  }

  lua_pushcfunction(lua, lua_errorhandler);
  lua_getglobal(lua, "love");
  lua_pushstring(lua, "load");
  lua_rawget(lua, -2);
  if(lua_pcall(lua, 0, 0, 1)) {
    printf("Errorin love.load: %s\n", lua_tostring(lua, -1));
  }
  lua_pop(lua, 1);

  lua_pushcfunction(lua, lua_errorhandler);
  MainLoopData mainLoopData = {
    .luaState = lua,
    .errhand = luaL_ref(lua, LUA_REGISTRYINDEX)
  };

  timer_init();
#ifdef EMSCRIPTEN
  emscripten_set_main_loop_arg(main_loop, &mainLoopData, 0, 1);
#else
  for(;;) {
    main_loop(&mainLoopData);
  }
#endif
}
