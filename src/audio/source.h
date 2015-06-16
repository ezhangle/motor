#pragma once

#include <AL/al.h>

typedef enum {
  audio_SourceState_playing,
  audio_SourceState_stopped,
  audio_SourceState_paused
} audio_SourceState;

typedef struct {
  ALuint source;
  audio_SourceState state;
} audio_SourceCommon;


void audio_SourceCommon_init(audio_SourceCommon *source);
void audio_SourceCommon_play(audio_SourceCommon *source);
void audio_SourceCommon_stop(audio_SourceCommon *source);
void audio_SourceCommon_pause(audio_SourceCommon *source);
void audio_SourceCommon_resume(audio_SourceCommon *source);
audio_SourceState audio_SourceCommon_getState(audio_SourceCommon const *source);
