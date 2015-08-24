#include <SDL_image.h>
#include "imagedata.h"

#ifndef EMSCRIPTEN
struct {
  SDL_PixelFormat * format;
 
} moduleData;
#endif

void image_ImageData_new_with_filename(image_ImageData *dst, char const* filename) {
  #ifndef EMSCRIPTEN
    SDL_Surface* surf = IMG_Load(filename);
    dst->surface = SDL_ConvertSurface(surf, moduleData.format, 0);
    SDL_FreeSurface(surf);
  #else
    dst->surface = IMG_Load(filename);
  #endif
}

void image_ImageData_new_with_size(image_ImageData *dst, int width, int height) {
  dst->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
}

void image_ImageData_free(image_ImageData *data) {
  SDL_FreeSurface(data->surface);
}

void image_init() {
  #ifndef EMSCRIPTEN
    moduleData.format = SDL_AllocFormat(SDL_PIXELFORMAT_ABGR8888);
  #endif
}
