#pragma once

#include <AL/al.h>

typedef struct {

  int channels;
  ALuint source;
  ALuint buffer;
} audio_Source;

void audio_init();
void audio_load(audio_Source *source, char const * filename);
void audio_Source_play(audio_Source const *source);
