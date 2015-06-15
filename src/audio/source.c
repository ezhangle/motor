#include "source.h"

void audio_SourceCommon_init(audio_SourceCommon *source) {
  alGenSources(1, &source->source);

  alSourcef(source->source, AL_PITCH, 1);
  alSourcef(source->source, AL_GAIN, 1);
  alSource3f(source->source, AL_POSITION, 0, 0, 0);
  alSource3f(source->source, AL_VELOCITY, 0, 0, 0);
  alSourcei(source->source, AL_LOOPING, AL_FALSE);

  source->state = audio_SourceState_stopped;
}


void audio_SourceCommon_play(audio_SourceCommon *source) {
  alSourcePlay(source->source);
  source->state = audio_SourceState_playing;
}


void audio_SourceCommon_stop(audio_SourceCommon *source) {
  alSourceStop(source->source);
  source->state = audio_SourceState_stopped;
}

audio_SourceState audio_SourceCommon_getState(audio_SourceCommon const *source) {
  ALint state;
  alGetSourcei(source->source, AL_SOURCE_STATE, &state);
  return state;
}


void audio_SourceCommon_pause(audio_SourceCommon *source) {
  if(source->common.state != audio_SourceState_playing) {
    return;
  }

  alSourcePause(source->source);
  source->state = audio_sourceState_paused;
}


void audio_SourceCommon_resume(audio_SourceCommon *source) {
  if(source->state == audio_SourceState_paused) {
    alSourcePlay(source->source);
    source->state = audio_sourceState_playing;
  }
}
