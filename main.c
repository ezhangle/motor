#include <SDL.h>
#include <stdio.h>
#include "lua/src/lua.h"
#include "lua/src/lauxlib.h"
#include "lua/src/lualib.h"

int main()
{
  lua_State *lua;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  SDL_CreateWindowAndRenderer(600, 600, SDL_WINDOW_OPENGL, &window, &renderer);

  lua = luaL_newstate();

  luaL_openlibs(lua);

  luaL_dostring(lua, "for i = 1, 100 do print('hallo', 'du', 'wurst', 5^9) end");

}
