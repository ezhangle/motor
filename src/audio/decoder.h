#pragma once

#include <stdbool.h>
#include <AL/al.h>

typedef struct {
  bool (*testFile)(char const *filename);
  bool (*loadFile)(ALuint buffer, char const* filename);
} audio_StaticSourceDecoder;

typedef struct {
  bool (*testFile)(char const *filename);
  bool (*openFile)(char const *filename, void **decoderData);
  int  (*getChannelCount)(void *decoderData);
  int  (*getSampleRate)(void *decoderData);
  bool (*closeFile)(void **decoderData);
  int  (*preloadSamples)(void *decoderData, int sampleCount);
  int  (*uploadPreloadedSamples)(void *decoderData, ALuint buffer);
} audio_StreamSourceDecoder;
