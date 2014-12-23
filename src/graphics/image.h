#pragma once

#include "../image/imagedata.h"

typedef struct {
  motor_image_ImageData* imageData;

} motor_graphics_Image;

void motor_graphics_image_new_with_imagedata(motor_graphics_Image *dst, motor_image_ImageData const *data);
void motor_graphics_image_new_with_filename(motor_graphics_Image *dst, char const *data);
void motor_graphics_image_free(motor_graphics_Image *obj);
