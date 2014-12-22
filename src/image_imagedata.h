#pragma once

#include <SDL/SDL.h>

typedef struct {
  SDL_Surface *surface;
} motor_image_ImageData;

void motor_image_imagedata_new_with_size(motor_image_ImageData *dst, int width, int height);
void motor_image_imagedata_new_with_filename(motor_image_ImageData *dst, char const* filename);
void motor_image_imagedata_free(motor_image_ImageData *data);
