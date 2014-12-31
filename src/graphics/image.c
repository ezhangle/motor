#include <stdio.h>
#include "image.h"
#include "../image/imagedata.h"

void graphics_Image_new_with_ImageData(graphics_Image *dst, image_ImageData const *data) {
  dst->imageData = data;
  glGenTextures(1, &dst->texID);
  glBindTexture(GL_TEXTURE_2D, dst->texID);

  graphics_Wrap wrap = {
    .verMode = graphics_WrapMode_repeat,
    .horMode = graphics_WrapMode_repeat
  };
  graphics_Image_setWrap(dst, &wrap);

  graphics_Filter filter = {
    .maxAnisotropy = 1.0f,
    .mipmapLodBias = 1.0f,
    .minMode = graphics_FilterMode_linear,
    .magMode = graphics_FilterMode_linear,
  };
  graphics_Image_setFilter(dst, &filter);
  printf("New image with imagedata: %p, %p\n", data, dst);
  graphics_Image_refresh(dst);
}

void graphics_Image_refresh(graphics_Image *img) {
  glBindTexture(GL_TEXTURE_2D, img->texID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->imageData->surface->w, img->imageData->surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->imageData->surface->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  img->width = img->imageData->surface->w;
  img->height = img->imageData->surface->h;
}

void graphics_Image_free(graphics_Image *obj) {
  glDeleteTextures(1, &obj->texID);
  printf("Deleting image %p\n", obj);
}

void graphics_Image_setFilter(graphics_Image *img, graphics_Filter const* filter) {
  glBindTexture(GL_TEXTURE_2D, img->texID);

  int minFilter = GL_NEAREST;
  if(filter->mipmapMode == graphics_FilterMode_none) {
    switch(filter->minMode) {
    case graphics_FilterMode_linear:
      minFilter = GL_LINEAR;
      break;
    case graphics_FilterMode_nearest:
      minFilter = GL_NEAREST;
      break;
    default:
      break;
    }
  } else if(filter->mipmapMode == graphics_FilterMode_nearest) {
    switch(filter->minMode) {
    case graphics_FilterMode_linear:
      minFilter = GL_LINEAR_MIPMAP_NEAREST;
      break;
    case graphics_FilterMode_nearest:
      minFilter = GL_NEAREST_MIPMAP_NEAREST;
      break;
    default:
      break;
    }
  } else if(filter->mipmapMode == graphics_FilterMode_linear) {
    switch(filter->minMode) {
    case graphics_FilterMode_linear:
      minFilter = GL_LINEAR_MIPMAP_LINEAR;
      break;
    case graphics_FilterMode_nearest:
      minFilter = GL_NEAREST_MIPMAP_LINEAR;
      break;
    default:
      break;
    }
  }

  int magFilter = (filter->magMode == graphics_FilterMode_linear) ? GL_LINEAR : GL_NEAREST;
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, filter->maxAnisotropy);
  
  // Not supported by WebGL, still interesting for native builds.
  // Accept GL_INVALID_ENUM on WebGL
  if(filter->mipmapMode != graphics_FilterMode_none) {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, filter->mipmapLodBias);
    printf("err: %x\n", glGetError());
  }
}

void graphics_Image_getFilter(graphics_Image *img, graphics_Filter *filter) {
  glBindTexture(GL_TEXTURE_2D, img->texID);
  int fil;
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,         &fil);
  switch(fil) {
  case GL_NEAREST:
    filter->minMode = graphics_FilterMode_nearest;
    filter->mipmapMode = graphics_FilterMode_none;
    break;
  case GL_LINEAR:
    filter->minMode = graphics_FilterMode_linear;
    filter->mipmapMode = graphics_FilterMode_none;
    break;
  case GL_NEAREST_MIPMAP_NEAREST:
    filter->minMode = graphics_FilterMode_nearest;
    filter->mipmapMode = graphics_FilterMode_nearest;
    break;
  case GL_NEAREST_MIPMAP_LINEAR:
    filter->minMode = graphics_FilterMode_nearest;
    filter->mipmapMode = graphics_FilterMode_linear;
    break;
  case GL_LINEAR_MIPMAP_NEAREST:
    filter->minMode = graphics_FilterMode_linear;
    filter->mipmapMode = graphics_FilterMode_nearest;
    break;
  case GL_LINEAR_MIPMAP_LINEAR:
    filter->minMode = graphics_FilterMode_linear;
    filter->mipmapMode = graphics_FilterMode_linear;
    break;
  }

  if(filter->mipmapMode == graphics_FilterMode_none) {
    filter->mipmapLodBias = 0.0f;
  } else {
    glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, &filter->mipmapLodBias);
  }

  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,         &fil);

  filter->magMode = (fil == GL_LINEAR) ? graphics_FilterMode_linear : graphics_FilterMode_nearest;
  glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &filter->maxAnisotropy);
}

void graphics_Image_setWrap(graphics_Image *img, graphics_Wrap const* wrap) {
  glBindTexture(GL_TEXTURE_2D, img->texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap->horMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap->verMode);
}

void graphics_Image_getWrap(graphics_Image *img, graphics_Wrap *wrap) {
  glBindTexture(GL_TEXTURE_2D, img->texID);
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int*)&wrap->horMode);
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int*)&wrap->verMode);
}
