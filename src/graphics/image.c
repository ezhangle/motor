#include <stdio.h>
#include "image.h"
#include "../image/imagedata.h"

void graphics_Image_new_with_ImageData(graphics_Image *dst, image_ImageData const *data) {
  printf("New image with imagedata: %p\n", data);
}

void graphics_Image_free(graphics_Image *obj) {
  
}
