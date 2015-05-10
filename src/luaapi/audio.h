#pragma once

#include <lua.h>
#include <stdbool.h>
#include "../audio/audio.h"
#include "../audio/streamsource.h"
#include "../audio/staticsource.h"

typedef enum {
  audio_SourceType_static,
  audio_SourceType_stream
} audio_SourceType;

typedef struct {
  audio_SourceType type;
  audio_StaticSource source;
} l_audio_StaticSource;

typedef struct {
  audio_SourceType type;
  audio_StreamSource source;
} l_audio_StreamSource;

int l_audio_register(lua_State *state);
bool l_audio_isSource(lua_State* state, int index);
