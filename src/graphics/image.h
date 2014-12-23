#pragma once

#include "../image/imagedata.h"

typedef struct {
  image_ImageData* imageData;

} graphics_Image;

void graphics_Image_new_with_ImageData(graphics_Image *dst, image_ImageData const *data);
void graphics_Image_new_with_filename(graphics_Image *dst, char const *data);
void graphics_Image_free(graphics_Image *obj);
