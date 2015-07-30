#pragma once

#include <stdbool.h>
#include <AL/al.h>

#include "source.h"
#include "staticbuffer.h"

typedef struct {
  audio_SourceCommon common;
  audio_StaticBuffer *buffer;
} audio_StaticSource;


// TODO free static source
void audio_loadStatic(audio_StaticSource *source, char const * filename);
void audio_StaticSource_play(audio_StaticSource *source);
void audio_StaticSource_setLooping(audio_StaticSource *source, bool loop);
void audio_StaticSource_stop(audio_StaticSource *source);
void audio_StaticSource_rewind(audio_StaticSource *source);
void audio_StaticSource_pause(audio_StaticSource *source);
void audio_StaticSource_resume(audio_StaticSource *source);
void audio_StaticSource_clone(audio_StaticSource const* oldSrc, audio_StaticSource * newSrc);
bool audio_StaticSource_isLooping(audio_StaticSource const* source);
