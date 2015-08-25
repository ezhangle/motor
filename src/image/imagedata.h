#pragma once

#include <stdint.h>

typedef struct {
  int w;
  int h;
  uint8_t *surface;
} image_ImageData;

void image_ImageData_new_with_size(image_ImageData *dst, int width, int height);
void image_ImageData_new_with_filename(image_ImageData *dst, char const* filename);
void image_ImageData_free(image_ImageData *data);
void image_init();
