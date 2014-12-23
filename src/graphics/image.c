#include <stdio.h>
#include "image.h"
#include "../image/imagedata.h"

void motor_graphics_image_new_with_imagedata(motor_graphics_Image *dst, motor_image_ImageData const *data) {
  printf("New image with imagedata: %p\n", data);
}

void motor_graphics_image_new_with_filename(motor_graphics_Image *dst, char const * filename) {
  printf("new image with filename: %s\n", filename);
}

void motor_graphics_image_free(motor_graphics_Image *obj) {
  
}
