#include "keyboard.h"
#include "../keyboard.h"

static struct {
  lua_State *luaState;

} moduleData;

void l_keyboard_register(lua_State* state) {
  moduleData.luaState = state;
}

void l_keyboard_keypressed(SDL_Keycode key, bool isrepeat) {
  lua_getglobal(moduleData.luaState, "motor");
  lua_pushstring(moduleData.luaState, "keypressed");
  lua_rawget(moduleData.luaState, -2);
  lua_pushstring(moduleData.luaState, keyboard_getKeyName(key));
  lua_pushboolean(moduleData.luaState, isrepeat);
  lua_call(moduleData.luaState, 2, 0);
}

void l_keyboard_keyreleased(SDL_Keycode key) {
  lua_getglobal(moduleData.luaState, "motor");
  lua_pushstring(moduleData.luaState, "keyreleased");
  lua_rawget(moduleData.luaState, -2);
  lua_pushstring(moduleData.luaState, keyboard_getKeyName(key));
  lua_call(moduleData.luaState, 1, 0);
}
