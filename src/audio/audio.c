#include "audio.h"
#include <AL/al.h>
#include <AL/alc.h>
#include "vorbis_decoder.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

static struct {
  ALCdevice  *device;
  ALCcontext *context;
} moduleData;

void audio_load(audio_Source *source, char const * filename) {
  // TODO detect file type
  audio_vorbis_load(source, filename);
}

void audio_Source_play(audio_Source const *source) {
  alSourcePlay(source->source);
}

void audio_init() {
  moduleData.device = alcOpenDevice(0);
  moduleData.context = alcCreateContext(moduleData.device, 0);
  if(!alcMakeContextCurrent(moduleData.context)) {
    printf("Failed to initialite audio context\n");
  }


//  alSourcePlay(source);

}
