#include "vorbis_decoder.h"
#include <AL/al.h>
#include <alloca.h>
#include "../3rdparty/stb/stb_vorbis.c"
#include "audio.h"


bool audio_vorbis_load(audio_Source *source, char const * filename) {
  short *data;
  int channels;
  int samplingrate;
  int len = stb_vorbis_decode_filename(filename, &channels, &samplingrate, &data);

  if(len == -1) {
    return false;
  }

  alGenSources(1, &source->source);

  alSourcef(source->source, AL_PITCH, 1);
  alSourcef(source->source, AL_GAIN, 1);
  alSource3f(source->source, AL_POSITION, 0, 0, 0);
  alSource3f(source->source, AL_VELOCITY, 0, 0, 0);
  alSourcei(source->source, AL_LOOPING, AL_FALSE);

  alGenBuffers(1, &source->buffer);

  alBufferData(source->buffer, channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, data, len * sizeof(short), samplingrate);

  alSourcei(source->source, AL_BUFFER, source->buffer);

  free(data);

  return true;
}
