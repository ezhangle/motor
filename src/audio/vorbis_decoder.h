#pragma once

#include <stdbool.h>
#include "audio.h"

bool audio_vorbis_load(audio_Source *source, char const * filename);
