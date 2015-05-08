#include "audio.h"
#include <AL/al.h>
#include <AL/alc.h>
#include "vorbis_decoder.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

extern audio_StreamSourceDecoder audio_vorbis_decoder;

static audio_StreamSourceDecoder const* streamDecoders[] = {
  &audio_vorbis_decoder
};

static struct {
  ALCdevice  *device;
  ALCcontext *context;
  audio_StreamSource ** playingStreams;
  int playingStreamSize;
  int playingStreamCount;
  ALuint unqueueBuffer[AUDIO_BUFFERS_PER_STREAM];
} moduleData;

void audio_loadStatic(audio_StaticSource *source, char const * filename) {
  // TODO detect file type
  audio_vorbis_load(source, filename);
}

void audio_StaticSource_play(audio_StaticSource const *source) {
  alSourcePlay(source->source);
}

static void audio_loadStreamSamples(audio_StreamSource const * source, ALuint buffer) {
  int readSamples = source->decoder->loadStreamSamples(
    source->decoderData,
    buffer,
    8000
  );

/*
  alBufferData(
    buffer,
    source->channels >= 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
    source->bufferData,
    readSamples * sizeof(short),
    source->sampleRate
  );
*/
}

bool audio_loadStream(audio_StreamSource *source, char const * filename) {
  // TODO select approprate decoder (there only one right now though!)
  source->decoder = streamDecoders[0];

  bool good = source->decoder->openFile(filename, &source->decoderData);
  if(!good) {
    return false;
  }

  alGenSources(1, &source->source);
  alSourcef( source->source, AL_PITCH,    1.0f);
  alSourcef( source->source, AL_GAIN,     1.0f);
  alSource3f(source->source, AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(source->source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSourcei( source->source, AL_LOOPING,  AL_FALSE);

  alGenBuffers(AUDIO_BUFFERS_PER_STREAM, source->buffers);
  source->nextBuffer = 0;

  source->channels   = source->decoder->getChannelCount(source->decoderData);
  source->sampleRate = source->decoder->getSampleRate(source->decoderData);
  source->bufferSamples = source->channels * source->sampleRate / 2;

  source->bufferData = malloc(sizeof(ALshort) * source->bufferSamples);

  for(int i = 0; i < 2; ++i) {
    audio_loadStreamSamples(source, source->buffers[i]);
  }

  return good;

}

void audio_StreamSource_play(audio_StreamSource *source) {
  // TODO security checks: not playing?

  alSourceQueueBuffers(source->source, 2, source->buffers);

  if(moduleData.playingStreamCount == moduleData.playingStreamSize) {
    moduleData.playingStreamSize = 2 * moduleData.playingStreamSize;
    moduleData.playingStreams = realloc(moduleData.playingStreams, moduleData.playingStreamSize*sizeof(audio_StreamSource*));
  }

  moduleData.playingStreams[moduleData.playingStreamCount] = source;
  ++moduleData.playingStreamCount;

  alSourcePlay(source->source);
}

void audio_updateStreams() {
  for(int i = 0; i < moduleData.playingStreamCount; ++i) {
    audio_StreamSource const* source = moduleData.playingStreams[i];
    ALuint src = source->source;
    ALint count;
    alGetSourcei(src, AL_BUFFERS_PROCESSED, &count);

    for(int j = 0; j < count; ++j) {
      ALuint buf;
      alSourceUnqueueBuffers(src, 1, &buf);
      extern double curtime();
      double t1 = curtime();
      audio_loadStreamSamples(source, buf);
      double t2 = curtime();
  //    printf("Load time: %0.6fs\n", t2 - t1);
      alSourceQueueBuffers(src, 1, &buf);
    }
  }
}

void audio_init() {
  moduleData.device = alcOpenDevice(0);
  moduleData.context = alcCreateContext(moduleData.device, 0);
  if(!alcMakeContextCurrent(moduleData.context)) {
    printf("Failed to initialite audio context\n");
  }

  moduleData.playingStreamCount = 0;
  moduleData.playingStreamSize  = 16;
  moduleData.playingStreams     = malloc(sizeof(audio_StreamSource*) * 16);

  audio_vorbis_decoder_init();
}
