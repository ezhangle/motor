#include "image.h"
#include "../image/imagedata.h"
#include "../math/vector.h"
#include "graphics.h"
#include "vertex.h"

static struct {
  GLuint imageVBO;
  GLuint imageIBO;
  GLuint imageVAO;
} moduleData;

void graphics_image_init() {
  glGenVertexArrays(1, &moduleData.imageVAO);
  glBindVertexArray(moduleData.imageVAO);
  glGenBuffers(1, &moduleData.imageVBO);
  glGenBuffers(1, &moduleData.imageIBO);

  graphics_Vertex const imageVertices[] = {
    {{0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
    {{1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
    {{0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
    {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}
  };

  unsigned char const imageIndices[] = { 0, 1, 2, 3 };

  glBindBuffer(GL_ARRAY_BUFFER, moduleData.imageVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(imageVertices), imageVertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moduleData.imageIBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(imageIndices), imageIndices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(graphics_Vertex), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(graphics_Vertex), (GLvoid const*)8);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(graphics_Vertex), (GLvoid const*)16);
}

static const graphics_Wrap defaultWrap = {
  .verMode = graphics_WrapMode_clamp,
  .horMode = graphics_WrapMode_clamp
};

static const graphics_Filter defaultFilter = {
  .maxAnisotropy = 1.0f,
  .mipmapLodBias = 1.0f,
  .minMode = graphics_FilterMode_linear,
  .magMode = graphics_FilterMode_linear,
  .mipmapMode = graphics_FilterMode_none
};

void graphics_Image_new_with_ImageData(graphics_Image *dst, image_ImageData *data) {
  dst->imageData = data;
  glGenTextures(1, &dst->texID);
  glBindTexture(GL_TEXTURE_2D, dst->texID);
  graphics_Image_setFilter(dst, &defaultFilter);

  graphics_Image_setWrap(dst, &defaultWrap);

  graphics_Image_refresh(dst);
}

void graphics_Image_refresh(graphics_Image *img) {
  glBindTexture(GL_TEXTURE_2D, img->texID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->imageData->surface->w, img->imageData->surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->imageData->surface->pixels);
  img->width = img->imageData->surface->w;
  img->height = img->imageData->surface->h;
}

void graphics_Image_free(graphics_Image *obj) {
  glDeleteTextures(1, &obj->texID);
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
  } else {
    if(filter->mipmapMode == graphics_FilterMode_nearest) {
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
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  int magFilter = (filter->magMode == graphics_FilterMode_linear) ? GL_LINEAR : GL_NEAREST;
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, filter->maxAnisotropy);
  
  // Not supported by WebGL, still interesting for native builds.
  // Accept GL_INVALID_ENUM on WebGL
  #ifndef EMSCRIPTEN
  if(filter->mipmapMode != graphics_FilterMode_none) {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, filter->mipmapLodBias);
  }
  #endif
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

  #ifndef EMSCRIPTEN
  if(filter->mipmapMode == graphics_FilterMode_none) {
  #endif
    filter->mipmapLodBias = 0.0f;
  #ifndef EMSCRIPTEN
  } else {
    glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, &filter->mipmapLodBias);
  }
  #endif

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

void graphics_Image_draw(graphics_Image const* image, graphics_Quad const* quad,
                         float x, float y, float r, float sx, float sy,
                         float ox, float oy, float kx, float ky) {

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, image->texID);
  mat4x4 tr2d;
  m4x4_new_transform2d(&tr2d, x, y, r, sx, sy, ox, oy, kx, ky, image->width * quad->w, image->height * quad->h);
  graphics_drawArray(quad, &tr2d, moduleData.imageVAO, moduleData.imageIBO, 4, GL_TRIANGLE_STRIP, GL_UNSIGNED_BYTE, graphics_getColorPtr());
  
}
