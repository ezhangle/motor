#pragma once

#include <stdbool.h>
#include <AL/al.h>

#include "decoder.h"

typedef struct {
  audio_StreamSourceDecoder const* decoder;
  void* decoderData;
  int channels;
  int sampleRate;
  ALuint source;
  ALuint buffers[2];
  ALshort *bufferData;
  int bufferSamples;
  int maxFrameSamples;
  int samplesRead;
  int nextBuffer;
} audio_StreamSource;

bool audio_loadStream(audio_StreamSource *source, char const * filename);
void audio_StreamSource_play(audio_StreamSource *source);
void audio_updateStreams(void);
void audio_streamsource_init(void);
