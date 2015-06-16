#include <stdlib.h>
#include "streamsource.h"
#include "decoder.h"

static struct {
  audio_StreamSource ** playingStreams;
  int playingStreamSize;
  int playingStreamCount;
} moduleData;

extern audio_StreamSourceDecoder audio_vorbis_decoder;

static audio_StreamSourceDecoder const* streamDecoders[] = {
  &audio_vorbis_decoder
};


static void initialPreload(audio_StreamSource *source) {
  for(int i = 0; i < 2; ++i) {
    source->decoder->preloadSamples(source->decoderData, 44100);
    source->decoder->uploadPreloadedSamples(source->decoderData, source->buffers[i]);
  }
}


bool audio_loadStream(audio_StreamSource *source, char const * filename) {
  // TODO select approprate decoder (there only one right now though!)
  source->decoder = streamDecoders[0];

  bool good = source->decoder->openFile(filename, &source->decoderData);
  if(!good) {
    return false;
  }

  audio_SourceCommon_init(&source->common);

  alGenBuffers(2, source->buffers);

  initialPreload(source);

  source->looping = false;

  return good;
}


static void prepareToPlay(audio_StreamSource *source) {
  if(source->common.state == audio_SourceState_playing) {
    return;
  } else if(source->common.state == audio_SourceState_paused) {
    audio_SourceCommon_play(&source->common);
    return;
  }

  alSourceQueueBuffers(source->common.source, 2, source->buffers);

  if(moduleData.playingStreamCount == moduleData.playingStreamSize) {
    moduleData.playingStreamSize = 2 * moduleData.playingStreamSize;
    moduleData.playingStreams = realloc(moduleData.playingStreams, moduleData.playingStreamSize*sizeof(audio_StreamSource*));
  }

  moduleData.playingStreams[moduleData.playingStreamCount] = source;
  ++moduleData.playingStreamCount;
}


void audio_StreamSource_play(audio_StreamSource *source) {
  switch(source->common.state) {
  case audio_SourceState_stopped:
    prepareToPlay(source);
    // Fall through
  case audio_SourceState_paused:
    audio_SourceCommon_play(&source->common);
    break;
  default:
    break;
  }
}


void audio_updateStreams(void) {
  for(int i = 0; i < moduleData.playingStreamCount;) {
    audio_StreamSource const* source = moduleData.playingStreams[i];

    int loaded = source->decoder->preloadSamples(source->decoderData, 8000);
    if(loaded == 0) {
      if(source->looping) {
        source->decoder->rewind(source->decoderData);
      } else {
        
      }
    }

    ALuint src = source->common.source;
    ALint count;
    ALint queued;
    ALint state;
    alGetSourcei(src, AL_BUFFERS_PROCESSED, &count);
    alGetSourcei(src, AL_BUFFERS_QUEUED, &queued);
    alGetSourcei(src, AL_SOURCE_STATE, &state);
  //  printf("%d buffers free, %d queued, state=%d\n", count, queued, state);

    for(int j = 0; j < count; ++j) {
      ALuint buf;
      alSourceUnqueueBuffers(src, 1, &buf);
      // This may cause preloading two full frames
      
      int uploaded = source->decoder->uploadPreloadedSamples(source->decoderData, buf);
      if(uploaded) {
        alSourceQueueBuffers(src, 1, &buf);
      }
    }

    alGetSourcei(src, AL_BUFFERS_QUEUED, &queued);
    if(state == AL_STOPPED && queued == 0) {
      --moduleData.playingStreamCount;
      moduleData.playingStreams[i] = moduleData.playingStreams[moduleData.playingStreamCount];
    } else {
      ++i;
    }
  }
}


void audio_StreamSource_setLooping(audio_StreamSource *source, bool loop) {
  source->looping = loop;
}


void audio_streamsource_init(void) {
  moduleData.playingStreamCount = 0;
  moduleData.playingStreamSize  = 16;
  moduleData.playingStreams     = malloc(sizeof(audio_StreamSource*) * 16);
}


void audio_StreamSource_stop(audio_StreamSource *source) {
  if(source->common.state == audio_SourceState_stopped) {
    return;
  }

  for(int i = 0; i < moduleData.playingStreamCount; ++i) {
    if(moduleData.playingStreams[i] == source) {
      --moduleData.playingStreamCount;
      moduleData.playingStreams[i] = moduleData.playingStreams[moduleData.playingStreamCount];
      break;
    }
  }

  audio_SourceCommon_stop(&source->common);

  ALint count;
  alGetSourcei(source->common.source, AL_BUFFERS_PROCESSED, &count);
  for(int j = 0; j < count; ++j) {
    ALuint buf;
    alSourceUnqueueBuffers(source->common.source, 1, &buf);
  }

  source->decoder->rewind(source->decoderData);
  source->decoder->flush(source->decoderData);
  initialPreload(source);
}


void audio_StreamSource_rewind(audio_StreamSource *source) {
  // TODO Skip to end of current buffers if playing or paused
  audio_SourceState state = source->common.state;

  audio_StreamSource_stop(source);

  if(state == audio_SourceState_playing) {
    audio_StreamSource_play(source);
  } else {
    prepareToPlay(source);
    source->common.state = audio_SourceState_paused;
  }
}


void audio_StreamSource_pause(audio_StreamSource *source) {
  audio_SourceCommon_pause(&source->common);
}

void audio_StreamSource_resume(audio_StreamSource *source) {
  audio_SourceCommon_resume(&source->common);
}
