#include "tools.h"
#include "audio.h"

// TODO use two separate metatables for streaming and static sources?

static struct {
  int staticMT;
  int streamMT;
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
      audio_StaticSource *src = lua_newuserdata(state, sizeof(audio_StaticSource));
      audio_loadStatic(src, filename);
      lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.staticMT);
      break;
    }

  case audio_SourceType_stream:
    {
      audio_StreamSource *src = lua_newuserdata(state, sizeof(audio_StreamSource));
      audio_loadStream(src, filename);
      lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.streamMT);
      break;
    }
  }
  

  lua_setmetatable(state, -2);

  return 1;
}

l_checkTypeFn(l_audio_isStaticSource, moduleData.staticMT)
l_checkTypeFn(l_audio_isStreamSource, moduleData.streamMT)


#define t_l_audio_source_generic(type, fun) \
  static int l_audio_ ## type ## Source_ ## fun(lua_State *state) { \
    l_assertType(state, 1, l_audio_is ## type ## Source);  \
    audio_ ## type ## Source *src = (audio_ ## type ## Source*) lua_touserdata(state, 1); \
    audio_ ## type ## Source_ ## fun(src); \
    return 0; \
  }

t_l_audio_source_generic(Static, play)
t_l_audio_source_generic(Stream, play)
t_l_audio_source_generic(Static, stop)
t_l_audio_source_generic(Stream, stop)
t_l_audio_source_generic(Static, rewind)
t_l_audio_source_generic(Stream, rewind)
t_l_audio_source_generic(Static, pause)
t_l_audio_source_generic(Stream, pause)
t_l_audio_source_generic(Static, resume)
t_l_audio_source_generic(Stream, resume)
#undef t_l_audio_source_generic


#define t_l_audio_source_setLooping(type) \
  static int l_audio_ ## type ## Source_setLooping(lua_State *state) { \
    l_assertType(state, 1, l_audio_is ## type ## Source);  \
    audio_ ## type ## Source *src = lua_touserdata(state, 1); \
    bool loop = l_tools_toBooleanOrError(state, 2); \
    audio_ ## type ## Source_setLooping(src, loop); \
    return 0; \
  }

t_l_audio_source_setLooping(Static)
t_l_audio_source_setLooping(Stream)
#undef t_l_audio_source_setLooping


#define t_sourceMetatableFuncs(type) \
  static luaL_Reg const type ## SourceMetatableFuncs[] = { \
    {"play",       l_audio_ ## type ## Source_play}, \
    {"stop",       l_audio_ ## type ## Source_stop}, \
    {"rewind",     l_audio_ ## type ## Source_rewind}, \
    {"pause",      l_audio_ ## type ## Source_pause}, \
    {"resume",     l_audio_ ## type ## Source_resume}, \
    {"setLooping", l_audio_ ## type ## Source_setLooping}, \
    {NULL, NULL} \
  };
t_sourceMetatableFuncs(Stream)
t_sourceMetatableFuncs(Static)
#undef t_sourceMetatableFuncs

#if 0
#define t_freeFunctionAdaptor(fun, args, rets) \
  static int l_audio_ ## fun (lua_State *state) { \
    lua_getmetatable(state, 1); \
    lua_pushstring(state, #fun); \
    lua_rawget(state, -2); \
    lua_insert(state, 1); \
    lua_pop(state, 2); \
    lua_call(state, args, rets); \
    return rets; \
  }

t_freeFunctionAdaptor(pause,  1, 0)
t_freeFunctionAdaptor(resume, 1, 0)
t_freeFunctionAdaptor(stop,   1, 0)
t_freeFunctionAdaptor(play,   1, 0)
t_freeFunctionAdaptor(rewind, 1, 0)
#endif

static luaL_Reg const regFuncs[] = {
  {"newSource", l_audio_newSource},
  #if 0
  {"pause",     l_audio_pause},
  {"play",      l_audio_play},
  {"stop",      l_audio_stop},
  {"resume",    l_audio_resume},
  {"rewind",    l_audio_rewind},
  #endif
  {NULL, NULL}
};

static char const bootScript[] =
  "for i, k in ipairs({\"play\", \"pause\", \"stop\", \"resume\", \"rewind\"}) do\n"
  "  love.audio[k] = function(src, ...)\n"
  "    src[k](src, ...)\n"
  "  end\n"
  "end\n";

static int registerFreeFunctionAdaptors(lua_State *state) {
  return luaL_dostring(state, bootScript);
}

int l_audio_register(lua_State *state) {
  l_tools_registerModule(state, "audio", regFuncs);
  moduleData.staticMT = l_tools_makeTypeMetatable(state, StaticSourceMetatableFuncs);
  moduleData.streamMT = l_tools_makeTypeMetatable(state, StreamSourceMetatableFuncs);

  if(registerFreeFunctionAdaptors(state)) {
    return 0;
  }

  return 1;
}
