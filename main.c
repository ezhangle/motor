#include <SDL.h>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#else
#include <time.h>
#endif
#include <stdio.h>
#include "lua/src/lua.h"
#include "lua/src/lauxlib.h"
#include "lua/src/lualib.h"

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

int main()
{
  lua_State *lua;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  SDL_CreateWindowAndRenderer(600, 600, SDL_WINDOW_OPENGL, &window, &renderer);

  lua = luaL_newstate();

  luaL_openlibs(lua);

  lua_pushcfunction(lua, &lua_curtime);
  lua_setglobal(lua, "gettime");

  luaL_dostring(lua, "local r = 0\nlocal start = gettime()\nfor i = 1, 10000000 do r = r + i end\nlocal stop = gettime()\nprint(stop - start)");

}
