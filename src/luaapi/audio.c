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


static bool isSource(lua_State * state, int index) {
  return l_audio_isStaticSource(state, index) || l_audio_isStreamSource(state,index);
}


// Yes I know it is not very beautiful to assume knowledge of the structure
// of audio_StreamSource and audio_StaticSource. I am using this hack to
// avoid duplication of the code for each of these functions (and for the
// actual backends).
// But since I'm aiming to minimize the download size of the compiled
// engine, I think it is legitimate to do so.
#define t_l_audio_SourceCommon_getBool(fun) \
  static int l_audio_SourceCommon_ ## fun(lua_State* state) { \
    l_assertType(state, 1, isSource); \
    audio_SourceCommon const* src = (audio_SourceCommon const*)lua_touserdata(state, 1); \
    lua_pushboolean(state, audio_SourceCommon_ ## fun(src)); \
    return 1; \
  }
t_l_audio_SourceCommon_getBool(isPlaying)
t_l_audio_SourceCommon_getBool(isStopped)
t_l_audio_SourceCommon_getBool(isPaused)
#undef t_l_audio_SourceCommon_getBool

static int l_audio_SourceCommon_isStatic(lua_State *state) {
  if(l_audio_isStaticSource(state, 1)) {
    lua_pushboolean(state, true);
  } else if(l_audio_isStreamSource(state, 1)) {
    lua_pushboolean(state, false);
  } else {
    lua_pushstring(state, "Expected Source");
    return lua_error(state);
  }
  return 1;
}


static int l_audio_SourceCommon_setVolume(lua_State *state) {
  l_assertType(state, 1, isSource);
  float gain = l_tools_toNumberOrError(state, 2);
  audio_SourceCommon *source = (audio_SourceCommon*)lua_touserdata(state, 1);
  audio_SourceCommon_setVolume(source, gain);
  return 0;
}

static int l_audio_SourceCommon_getVolume(lua_State *state) {
  l_assertType(state, 1, isSource);
  audio_SourceCommon *source = (audio_SourceCommon*)lua_touserdata(state, 1);
  lua_pushnumber(state, audio_SourceCommon_getVolume(source));
  return 1;
}

#define t_sourceMetatableFuncs(type) \
  static luaL_Reg const type ## SourceMetatableFuncs[] = { \
    {"play",       l_audio_ ## type ## Source_play}, \
    {"stop",       l_audio_ ## type ## Source_stop}, \
    {"rewind",     l_audio_ ## type ## Source_rewind}, \
    {"pause",      l_audio_ ## type ## Source_pause}, \
    {"resume",     l_audio_ ## type ## Source_resume}, \
    {"setLooping", l_audio_ ## type ## Source_setLooping}, \
    {"isPlaying",  l_audio_SourceCommon_isPlaying}, \
    {"isStopped",  l_audio_SourceCommon_isStopped}, \
    {"isPaused",   l_audio_SourceCommon_isPaused}, \
    {"isStatic",   l_audio_SourceCommon_isStatic}, \
    {"setVolume",  l_audio_SourceCommon_setVolume}, \
    {"getVolume",  l_audio_SourceCommon_getVolume}, \
    {NULL, NULL} \
  };
t_sourceMetatableFuncs(Stream)
t_sourceMetatableFuncs(Static)
#undef t_sourceMetatableFuncs




static luaL_Reg const regFuncs[] = {
  {"newSource", l_audio_newSource},
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
