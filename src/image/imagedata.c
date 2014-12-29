#include <SDL_image.h>
#include <stdio.h>
#include "imagedata.h"


void image_ImageData_new_with_filename(image_ImageData *dst, char const* filename) {
  printf("new ImageData(filename): %s, %p\n", filename, dst);
  dst->surface = IMG_Load(filename);
}

void image_ImageData_new_with_size(image_ImageData *dst, int width, int height) {
  printf("new ImageData(width, height): %d, %d\n", width, height);
  dst->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
}

void image_ImageData_free(image_ImageData *data) {
  printf("Freeing imagedata: %p\n", data);
  SDL_FreeSurface(data->surface);
}
