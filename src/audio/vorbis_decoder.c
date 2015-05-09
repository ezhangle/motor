#include "vorbis_decoder.h"
#include <AL/al.h>
#include <alloca.h>
#include "../3rdparty/stb/stb_vorbis.c"
#include "audio.h"


typedef struct {
  stb_vorbis *vorbis;
  ALshort    *readBuffer;
  int         readBufferSize;
  int         preloadedSamples;
} audio_vorbis_DecoderData;

bool audio_vorbis_openStream(char const * filename, void **decoderData) {
  int err;
  audio_vorbis_DecoderData* data = malloc(sizeof(audio_vorbis_DecoderData));
  data->vorbis = stb_vorbis_open_filename(filename, &err, NULL);

  stb_vorbis_info info = stb_vorbis_get_info(data->vorbis);

  data->readBufferSize    = info.channels * info.sample_rate + 4096;
  data->readBuffer        = malloc(sizeof(ALshort) * data->readBufferSize);
  data->preloadedSamples  = 0;

  *decoderData = data;

  return true;
}


int audio_vorbis_preloadStreamSamples(void* decoderData, int sampleCount) {
  audio_vorbis_DecoderData * data = (audio_vorbis_DecoderData*)decoderData;
  stb_vorbis_info info = stb_vorbis_get_info(data->vorbis);
  int channels = info.channels >= 2 ? 2 : 1;   // Force to mono or stereo

  int safeBufferSize = sampleCount * channels + 4096;
  if(safeBufferSize > data->readBufferSize) {
    data->readBufferSize = safeBufferSize;
    free(data->readBuffer);
    data->readBuffer = malloc(sizeof(ALshort) * safeBufferSize);
  }

  int space = data->readBufferSize - data->preloadedSamples - 4096;
  #if 0
  if(space <= 0) {
    printf("Buffer already 100%% filled, no need to read more\n");
  }
  #endif

  if(space < sampleCount) {
    sampleCount = space;
  }

  int readSamples = 0;
  while(readSamples < sampleCount) {
    float **channelData;
    int samples = stb_vorbis_get_frame_float(data->vorbis, NULL, &channelData);
    for(int i = 0; i < samples; ++i) {
      for(int c = 0; c < channels; ++c) {
        data->readBuffer[data->preloadedSamples + readSamples + channels * i + c] = (ALshort)(channelData[c][i] * 0x7FFF);
      }
    }

    readSamples += channels * samples;
  }

  data->preloadedSamples += readSamples;
  //printf("Preloaded %d samples, buffered: %d samples\n", readSamples, data->preloadedSamples);

  return readSamples;
}

int audio_vorbis_uploadPreloadedStreamSamples(void *decoderData, ALuint buffer) {
  audio_vorbis_DecoderData * data = (audio_vorbis_DecoderData*)decoderData;
  stb_vorbis_info info = stb_vorbis_get_info(data->vorbis);
  int channels = info.channels >= 2 ? 2 : 1;   // Force to mono or stereo

  // Emergency loading if we ran out of time for proper preloading
  if(data->preloadedSamples < data->readBufferSize / 2) {
    audio_vorbis_preloadStreamSamples(decoderData, data->readBufferSize / 2);
  }

  alBufferData(
    buffer,
    channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
    data->readBuffer,
    data->preloadedSamples * sizeof(ALshort),
    info.sample_rate
  );
  //printf("uploaded %d samples to buffer %d\n", data->preloadedSamples, buffer);

  data->preloadedSamples = 0;
  return 0;
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
  .preloadSamples    = audio_vorbis_preloadStreamSamples,
  .uploadPreloadedSamples = audio_vorbis_uploadPreloadedStreamSamples
};
