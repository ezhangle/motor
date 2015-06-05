#pragma once

#include <stdbool.h>
#include <AL/al.h>

typedef struct {
  bool (*testFile)(char const *filename);
  bool (*loadFile)(ALuint buffer, char const* filename);
} audio_StaticSourceDecoder;

/*
  preloadSamples must return:  0 if end of stream reached
                              -1 if buffer is full
                               n where n > 0 if n samples were preloaded
*/

typedef struct {
  bool (*testFile)(char const *filename);
  bool (*openFile)(char const *filename, void **decoderData);
  int  (*getChannelCount)(void *decoderData);
  int  (*getSampleRate)(void *decoderData);
  bool (*closeFile)(void **decoderData);
  bool (*atEnd)(void const *decoderData);
  void (*rewind)(void *decoderData);
  int  (*preloadSamples)(void *decoderData, int sampleCount);
  int  (*uploadPreloadedSamples)(void *decoderData, ALuint buffer);
  void (*flush)(void *decoderData);
} audio_StreamSourceDecoder;
