#include "keyboard.h"
#include "../keyboard.h"
#include "tools.h"

static struct {
  bool keyRepeat;

} moduleData;


int l_keyboard_hasKeyRepeat(lua_State* state) {
  lua_pushboolean(state, moduleData.keyRepeat);
  return 1;
}

int l_keyboard_setKeyRepeat(lua_State* state) {
  moduleData.keyRepeat = lua_toboolean(state, 1);
  return 0;
}

int l_keyboard_isDown(lua_State* state) {
  bool any = false;
  int top = lua_gettop(state); 
  for(int i = 0; i < top; ++i) {
    any = any || keyboard_ispressed(
                  keyboard_getKeycode(
                    l_tools_toStringOrError(state, i + 1)));
    if(any) {
      break;
    }
  }

  lua_pushboolean(state, any);
  return 1;
}

int l_keyboard_setTextInput(lua_State* state) {
  bool enable = lua_toboolean(state, 1);
  if(enable) {
    keyboard_startText();
  } else {
    keyboard_stopText();
  }
  return 0;
}

int l_keyboard_hasTextInput(lua_State* state) {
  lua_pushboolean(state, keyboard_isTextEnabled());
  return 1;
}

static luaL_Reg const regFuncs[] = {
  {"hasKeyRepeat", l_keyboard_hasKeyRepeat},
  {"setKeyRepeat", l_keyboard_setKeyRepeat},
  {"isDown",       l_keyboard_isDown},
  {"setTextInput", l_keyboard_setTextInput},
  {"hasTextInput", l_keyboard_hasTextInput},
  {NULL, NULL}
};


static void l_keyboard_keypressed(lua_State* state, SDL_Keycode key, bool isrepeat) {
  if(isrepeat && !moduleData.keyRepeat) {
    return;
  }
  lua_getglobal(state, "love");
  lua_pushstring(state, "keypressed");
  lua_rawget(state, -2);
  lua_pushstring(state, keyboard_getKeyName(key));
  lua_pushboolean(state, isrepeat);
  lua_call(state, 2, 0);
}


static void l_keyboard_keyreleased(lua_State * state, SDL_Keycode key) {
  lua_getglobal(state, "love");
  lua_pushstring(state, "keyreleased");
  lua_rawget(state, -2);
  lua_pushstring(state, keyboard_getKeyName(key));
  lua_call(state, 1, 0);
}


static void l_keyboard_textInput(lua_State * state, char const* text) {
  lua_getglobal(state, "love");
  lua_pushstring(state, "textinput");
  lua_rawget(state, -2);
  lua_pushstring(state, text);
  lua_call(state, 1, 0);
}


void l_keyboard_register(lua_State* state) {
  moduleData.keyRepeat = false;

  l_tools_registerModule(state, "keyboard", regFuncs);

  keyboard_EventCallbacks callbacks = {
    .pressed = l_keyboard_keypressed,
    .released = l_keyboard_keyreleased,
    .textInput = l_keyboard_textInput,
    .userData = state
  };

  keyboard_setEventCallbacks(&callbacks);
}

