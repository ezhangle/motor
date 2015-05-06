#include "audio.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

static struct {
  ALCdevice  *device;
  ALCcontext *context;
} moduleData;

void audio_init() {
  moduleData.device = alcOpenDevice(0);
  moduleData.context = alcCreateContext(moduleData.device, 0);
  if(!alcMakeContextCurrent(moduleData.context)) {
    printf("Failed to initialite audio context\n");
  }

  ALuint source;
  alGenSources(1, &source);

  alSourcef(source, AL_PITCH, 1);
  alSourcef(source, AL_GAIN, 1);
  alSource3f(source, AL_POSITION, 0, 0, 0);
  alSource3f(source, AL_VELOCITY, 0, 0, 0);
  alSourcei(source, AL_LOOPING, AL_FALSE);

  ALuint buffer;
  alGenBuffers(1, &buffer);

  int16_t *buf= malloc(sizeof(int16_t) * 200000);

  for(int i = 0; i < 100000; ++i) {
    buf[2*i] = buf[2*i+1] = 16000 * sinf((i*i / 100000.0f) * 2.0f * 3.14159);
  }

  alBufferData(buffer, AL_FORMAT_STEREO16, buf, 400000, 44100);

  alSourcei(source, AL_BUFFER, buffer);

  alSourcePlay(source);

  free(buf);
}
