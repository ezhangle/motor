#include "vorbis_decoder.h"
#include <AL/al.h>
#include <alloca.h>
#include "../3rdparty/stb/stb_vorbis.c"
#include "audio.h"

static struct {
  short *streamReadBuffer;
  int currentStreamReadBufferSize;
} moduleData;

bool audio_vorbis_openStream(char const * filename, void **decoderData) {
  int err;
  stb_vorbis *vorbis = stb_vorbis_open_filename(filename, &err, NULL);

  stb_vorbis_info info = stb_vorbis_get_info(vorbis);

  *decoderData = vorbis;

  if(moduleData.currentStreamReadBufferSize < info.max_frame_size) {
    free(moduleData.streamReadBuffer);
    moduleData.streamReadBuffer = malloc(sizeof(short) * info.max_frame_size);
    moduleData.currentStreamReadBufferSize = info.max_frame_size;
  }

  return true;
}


int audio_vorbis_loadStreamSamples(void *decoderData, ALshort *buffer, int maxSamples) {
  stb_vorbis * vorbis = (stb_vorbis*)decoderData;
  stb_vorbis_info info = stb_vorbis_get_info(vorbis);
  int channels = info.channels >= 2 ? 2 : 1;

  int readSamples = 0;
  while(maxSamples - readSamples >= channels * 4096) {
    float **data;
    int samples = stb_vorbis_get_frame_float(vorbis, NULL, &data);
    for(int i = 0; i < samples; ++i) {
      for(int c = 0; c < channels; ++c) {
        buffer[readSamples + channels * i + c] = (ALshort)(data[c][i] * 0x7FFF);
      }
    }

    readSamples += channels * samples;
  }

  return readSamples;
}


bool audio_vorbis_load(audio_StaticSource *source, char const * filename) {
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

void audio_vorbis_decoder_init() {
  moduleData.streamReadBuffer = 0;
  moduleData.currentStreamReadBufferSize = 0;
}

int audio_vorbis_getChannelCount(void *decoderData) {
  stb_vorbis_info info = stb_vorbis_get_info((stb_vorbis*)decoderData);
  return info.channels;
}

int audio_vorbis_getSampleRate(void *decoderData) {
  stb_vorbis_info info = stb_vorbis_get_info((stb_vorbis*)decoderData);
  return info.sample_rate;
}

audio_StreamSourceDecoder audio_vorbis_decoder = {
  .testFile          = NULL,
  .getChannelCount   = audio_vorbis_getChannelCount,
  .getSampleRate     = audio_vorbis_getSampleRate,
  .openFile          = audio_vorbis_openStream,
  .closeFile         = NULL,
  .loadStreamSamples = audio_vorbis_loadStreamSamples
};
