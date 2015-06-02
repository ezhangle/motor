#include "tools.h"
#include "audio.h"

// TODO use two separate metatables for streaming and static sources?

static struct {
  int sourceMT;
} moduleData;


static const l_tools_Enum l_audio_SourceType[] = {
  {"static", audio_SourceType_static},
  {"stream", audio_SourceType_stream},
  {NULL, 0}
};

static int l_audio_newSource(lua_State *state) {
  char const* filename = l_tools_toStringOrError(state, 1);

  audio_SourceType type = audio_SourceType_stream;
  if(!lua_isnoneornil(state, 2)) {
    type = l_tools_toEnumOrError(state, 2, l_audio_SourceType);
  }

  // TODO handle load errors
  switch(type) {
  case audio_SourceType_static:
    {
      l_audio_StaticSource *src = lua_newuserdata(state, sizeof(l_audio_StaticSource));
      src->type = audio_SourceType_static;
      audio_loadStatic(&src->source, filename);
      break;
    }

  case audio_SourceType_stream:
    {
      l_audio_StreamSource *src = lua_newuserdata(state, sizeof(l_audio_StreamSource));
      src->type = audio_SourceType_stream;
      audio_loadStream(&src->source, filename);
      break;
    }
  }
  

  lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.sourceMT);
  lua_setmetatable(state, -2);

  return 1;
}

l_checkTypeFn(l_audio_isSource, moduleData.sourceMT)

static int l_audio_Source_play(lua_State *state) {
  l_assertType(state, 1, l_audio_isSource); 
  audio_SourceType *type = lua_touserdata(state, 1);
  switch(*type) {
  case audio_SourceType_static:
    {
      l_audio_StaticSource *src = (l_audio_StaticSource*)type;
      audio_StaticSource_play(&src->source);
      break;
    }

  case audio_SourceType_stream:
    {
      l_audio_StreamSource *src = (l_audio_StreamSource*)type;
      audio_StreamSource_play(&src->source);
      break;
    }
  }

  return 0;
}

static int l_audio_source_setLooping(lua_State *state) {
  l_assertType(state, 1, l_audio_isSource); 
  audio_SourceType *type = lua_touserdata(state, 1);
  bool loop = l_tools_toBooleanOrError(state, 2);
  switch(*type) {
  case audio_SourceType_static:
    {
      l_audio_StaticSource *src = (l_audio_StaticSource*)type;
      audio_StaticSource_setLooping(&src->source, loop);
      break;
    }

  case audio_SourceType_stream:
    {
      l_audio_StreamSource *src = (l_audio_StreamSource*)type;
      audio_StreamSource_setLooping(&src->source, loop);
      break;
    }
  }

  return 0;
}

static luaL_Reg const sourceMetatableFuncs[] = {
  {"play", l_audio_Source_play},
  {"setLooping", l_audio_source_setLooping},
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
