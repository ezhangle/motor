#pragma once

#include <stdbool.h>
#include <AL/al.h>


typedef struct {
  int channels;
  ALuint source;
  ALuint buffer;
} audio_StaticSource;


void audio_loadStatic(audio_StaticSource *source, char const * filename);
void audio_StaticSource_play(audio_StaticSource const *source);
