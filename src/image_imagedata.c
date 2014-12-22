#include <SDL_image.h>
#include <stdio.h>
#include "image_imagedata.h"


void motor_image_imagedata_new_with_filename(motor_image_ImageData *dst, char const* filename) {
  printf("new ImageData(filename): %s\n", filename);
  dst->surface = IMG_Load(filename);
}

void motor_image_imagedata_new_with_size(motor_image_ImageData *dst, int width, int height) {
  printf("new ImageData(width, height): %d, %d\n", width, height);
  dst->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
}

void motor_image_imagedata_free(motor_image_ImageData *data) {
  SDL_FreeSurface(data->surface);
}
