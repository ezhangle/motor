#include "tools.h"
#include "audio.h"

static struct {
  int sourceMT;
} moduleData;

static int l_audio_newSource(lua_State *state) {
  char const* filename = l_tools_toStringOrError(state, 1);

  audio_Source *src = lua_newuserdata(state, sizeof(audio_Source));
  
  audio_load(src, filename);

  lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.sourceMT);
  lua_setmetatable(state, -2);

  return 1;
}

l_checkTypeFn(l_audio_isSource, moduleData.sourceMT)
l_toTypeFn(l_audio_toSource, audio_Source)

static int l_audio_Source_play(lua_State *state) {
  l_assertType(state, 1, l_audio_isSource); 
  audio_Source const* source = l_audio_toSource(state, 1);

  audio_Source_play(source);

  return 0;
}

static luaL_Reg const sourceMetatableFuncs[] = {
  {"play", l_audio_Source_play},
  {NULL, NULL}
};


static luaL_Reg const regFuncs[] = {
  {"newSource", l_audio_newSource},
  {NULL, NULL}
};

int l_audio_register(lua_State *state) {
  l_tools_registerModule(state, "audio", regFuncs);
  moduleData.sourceMT = l_tools_makeTypeMetatable(state, sourceMetatableFuncs);

  return 1;
}
