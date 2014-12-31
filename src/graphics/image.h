#pragma once

#include "../image/imagedata.h"
#include <SDL/SDL_opengl.h>

typedef enum {
  graphics_FilterMode_none = 0,
  graphics_FilterMode_nearest = 1,
  graphics_FilterMode_linear = 2
} graphics_FilterMode;

typedef enum {
  graphics_WrapMode_clamp = GL_CLAMP_TO_EDGE,
  graphics_WrapMode_repeat = GL_REPEAT
} graphics_WrapMode;

typedef struct {
  float maxAnisotropy;
  float mipmapLodBias;
  graphics_FilterMode minMode;
  graphics_FilterMode magMode;
  graphics_FilterMode mipmapMode;
} graphics_Filter;

typedef struct {
  graphics_WrapMode verMode;
  graphics_WrapMode horMode;
} graphics_Wrap;

typedef struct {
  image_ImageData const * imageData;
  GLuint texID;
  int width;
  int height;
} graphics_Image;


void graphics_Image_new_with_ImageData(graphics_Image *dst, image_ImageData const *data);
void graphics_Image_free(graphics_Image *obj);
void graphics_Image_setFilter(graphics_Image *img, graphics_Filter const* filter);
void graphics_Image_getFilter(graphics_Image *img, graphics_Filter *filter);
void graphics_Image_setWrap(graphics_Image *img, graphics_Wrap const* wrap);
void graphics_Image_getWrap(graphics_Image *img, graphics_Wrap *wrap);
void graphics_Image_refresh(graphics_Image *img);
