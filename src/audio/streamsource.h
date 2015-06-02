#pragma once

#include <stdbool.h>
#include <AL/al.h>

#include "decoder.h"

typedef struct {
  audio_StreamSourceDecoder const* decoder;
  void*  decoderData;
  ALuint source;
  ALuint buffers[2];
  bool   looping;
} audio_StreamSource;

bool audio_loadStream(audio_StreamSource *source, char const * filename);
void audio_StreamSource_play(audio_StreamSource *source);
void audio_StreamSource_setLooping(audio_StreamSource *source, bool loop);
void audio_updateStreams(void);
void audio_streamsource_init(void);
