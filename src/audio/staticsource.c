#include "staticsource.h"
#include "decoder.h"

// TODO Put into external unit and generate that unit automatically during build
extern audio_StaticSourceDecoder audio_vorbis_static_decoder;
static audio_StaticSourceDecoder const *staticDecoders[] = {
  &audio_vorbis_static_decoder
};

void audio_loadStatic(audio_StaticSource *source, char const * filename) {
  audio_SourceCommon_init(&source->common);

//  alGenBuffers(1, &source->buffer);
  source->buffer = audio_StaticBuffer_new();

  // TODO detect file type
  staticDecoders[0]->loadFile(source->buffer->buffer, filename);

  alSourcei(source->common.source, AL_BUFFER, source->buffer->buffer);
}


void audio_StaticSource_play(audio_StaticSource *source) {
  if(source->common.state != audio_SourceState_playing) {
    audio_SourceCommon_play(&source->common);
  }
}


void audio_StaticSource_setLooping(audio_StaticSource *source, bool loop) {
  alSourcei(source->common.source, AL_LOOPING, loop);
}


bool audio_StaticSource_isLooping(audio_StaticSource const* source) {
  ALint loop;
  alGetSourcei(source->common.source, AL_LOOPING, &loop);
  return loop;
}


void audio_StaticSource_stop(audio_StaticSource *source) {
  audio_SourceCommon_stop(&source->common);
  audio_StaticSource_rewind(source);
}


void audio_StaticSource_rewind(audio_StaticSource *source) {
  alSourceRewind(source->common.source);

  if(source->common.state == audio_SourceState_playing) {
    audio_SourceCommon_play(&source->common);
  }
}


void audio_StaticSource_pause(audio_StaticSource *source) {
  audio_SourceCommon_pause(&source->common);
}


void audio_StaticSource_resume(audio_StaticSource *source) {
  audio_SourceCommon_resume(&source->common);
}


void audio_StaticSource_clone(audio_StaticSource const* oldSrc, audio_StaticSource * newSrc) {
  audio_SourceCommon_init(&newSrc->common);

  audio_StaticBuffer_ref(oldSrc->buffer);
  newSrc->buffer = oldSrc->buffer;

  alSourcei(newSrc->common.source, AL_BUFFER, newSrc->buffer->buffer);
}
