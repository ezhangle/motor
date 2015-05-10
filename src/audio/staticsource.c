#include "staticsource.h"
#include "decoder.h"

// TODO Put into external unit and generate that unit automatically during build
extern audio_StaticSourceDecoder audio_vorbis_static_decoder;
static audio_StaticSourceDecoder const *staticDecoders[] = {
  &audio_vorbis_static_decoder
};

void audio_loadStatic(audio_StaticSource *source, char const * filename) {
  alGenSources(1, &source->source);

  alSourcef(source->source, AL_PITCH, 1);
  alSourcef(source->source, AL_GAIN, 1);
  alSource3f(source->source, AL_POSITION, 0, 0, 0);
  alSource3f(source->source, AL_VELOCITY, 0, 0, 0);
  alSourcei(source->source, AL_LOOPING, AL_FALSE);

  alGenBuffers(1, &source->buffer);

  // TODO detect file type
  staticDecoders[0]->loadFile(source->buffer, filename);

  alSourcei(source->source, AL_BUFFER, source->buffer);
}


void audio_StaticSource_play(audio_StaticSource const *source) {
  alSourcePlay(source->source);
}
