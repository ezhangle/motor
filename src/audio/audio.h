#pragma once

#include <AL/al.h>
#include <stdbool.h>

typedef struct {

  int channels;
  ALuint source;
  ALuint buffer;
} audio_StaticSource;

typedef struct {
  bool (*testFile)(char const *filename);
  bool (*openFile)(char const *filename, void **decoderData);
  int  (*getChannelCount)(void *decoderData);
  int  (*getSampleRate)(void *decoderData);
  bool (*closeFile)(void **decoderData);
  int  (*preloadSamples)(void *decoderData, int sampleCount);
  int  (*uploadPreloadedSamples)(void *decoderData, ALuint buffer);
} audio_StreamSourceDecoder;

static int const AUDIO_BUFFERS_PER_STREAM = 8;

typedef struct {
  audio_StreamSourceDecoder const* decoder;
  void* decoderData;
  int channels;
  int sampleRate;
  ALuint source;
  ALuint buffers[AUDIO_BUFFERS_PER_STREAM];
  ALshort *bufferData;
  int bufferSamples;
  int maxFrameSamples;
  int samplesRead;
  int nextBuffer;
} audio_StreamSource;

void audio_init(void);
void audio_loadStatic(audio_StaticSource *source, char const * filename);
bool audio_loadStream(audio_StreamSource *source, char const * filename);
void audio_StaticSource_play(audio_StaticSource const *source);
void audio_StreamSource_play(audio_StreamSource *source);
void audio_updateStreams(void);
