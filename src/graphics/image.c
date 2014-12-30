#include <stdio.h>
#include "image.h"
#include "../image/imagedata.h"

void graphics_Image_new_with_ImageData(graphics_Image *dst, image_ImageData const *data) {
  dst->imageData = data;
  glGenTextures(1, &dst->texID);
  glBindTexture(GL_TEXTURE_2D, dst->texID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data->surface->w, data->surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data->surface->pixels);
  graphics_Image_setWrap(dst, graphics_WrapMode_repeat, graphics_WrapMode_repeat);
  graphics_Image_setFilter(dst, graphics_FilterMode_linear, graphics_FilterMode_linear);
  glGenerateMipmap(GL_TEXTURE_2D);
  dst->width = data->surface->w;
  dst->height = data->surface->h;
  printf("New image with imagedata: %p, %p\n", data, dst);
}

void graphics_Image_free(graphics_Image *obj) {
  glDeleteTextures(1, &obj->texID);
  printf("Deleting image %p\n", obj);
}

void graphics_Image_setFilter(graphics_Image *img, graphics_FilterMode minMode, graphics_FilterMode magMode) {
  glBindTexture(GL_TEXTURE_2D, img->texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode);

  img->filterMinMode = minMode;
  img->filterMagMode = magMode;
}

void graphics_Image_getFilter(graphics_Image *img, graphics_FilterMode* minMode, graphics_FilterMode* magMode) {
  *minMode = img->filterMinMode;
  *magMode = img->filterMagMode;
}

void graphics_Image_setWrap(graphics_Image *img, graphics_WrapMode horMode, graphics_WrapMode verMode) {
  glBindTexture(GL_TEXTURE_2D, img->texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verMode);
  
  img->wrapHorMode = horMode;
  img->wrapVerMode = verMode;
}

void graphics_Image_getWrap(graphics_Image *img, graphics_WrapMode *horMode, graphics_WrapMode *verMode) {
  *horMode = img->wrapHorMode;
  *verMode = img->wrapVerMode;
}
