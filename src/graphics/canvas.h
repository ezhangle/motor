#pragma once

#include <SDL/SDL_opengl.h>
#include "../math/vector.h"
#include "quad.h"

typedef struct {
  GLuint texture;
  GLuint fbo;

  mat4x4 projectionMatrix;

  int width;
  int height;
} graphics_Canvas;

void graphics_Canvas_new(graphics_Canvas *canvas, int width, int height);
void graphics_Canvas_free(graphics_Canvas *canvas);
void graphics_Canvas_draw(graphics_Canvas const* canvas, graphics_Quad const* quad,
                         float x, float y, float r, float sx, float sy,
                         float ox, float oy, float kx, float ky);
