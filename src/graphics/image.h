#pragma once

#include "../image/imagedata.h"
#include <SDL/SDL_opengl.h>

typedef enum {
  graphics_FilterMode_nearest = GL_NEAREST,
  graphics_FilterMode_linear = GL_LINEAR
} graphics_FilterMode;

typedef enum {
  graphics_WrapMode_clamp = GL_CLAMP_TO_EDGE,
  graphics_WrapMode_repeat = GL_REPEAT
} graphics_WrapMode;

typedef struct {
  image_ImageData const * imageData;
  GLuint texID;
  int width;
  int height;
  graphics_FilterMode filterMinMode;
  graphics_FilterMode filterMagMode;
  graphics_WrapMode   wrapVerMode;
  graphics_WrapMode   wrapHorMode;
} graphics_Image;


void graphics_Image_new_with_ImageData(graphics_Image *dst, image_ImageData const *data);
void graphics_Image_new_with_filename(graphics_Image *dst, char const *data);
void graphics_Image_free(graphics_Image *obj);
void graphics_Image_setFilter(graphics_Image *img, graphics_FilterMode minMode, graphics_FilterMode magMode);
void graphics_Image_getFilter(graphics_Image *img, graphics_FilterMode* minMode, graphics_FilterMode* magMode);
void graphics_Image_setWrap(graphics_Image *img, graphics_WrapMode horMode, graphics_WrapMode verMode);
void graphics_Image_getWrap(graphics_Image *img, graphics_WrapMode *horMode, graphics_WrapMode *verMode);
