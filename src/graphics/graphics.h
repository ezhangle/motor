#pragma once

#include <stdbool.h>
#include "image.h"
#include "quad.h"
#include "../math/vector.h"
#include "canvas.h"

void graphics_init(int width, int height);

typedef enum {
  graphics_BlendMode_additive,
  graphics_BlendMode_alpha,
  graphics_BlendMode_subtractive,
  graphics_BlendMode_multiplicative,
  graphics_BlendMode_premultiplied,
  graphics_BlendMode_replace,
  graphics_BlendMode_screen
} graphics_BlendMode;

void graphics_setBackgroundColor(float red, float green, float blue, float alpha);
void graphics_setColor(float red, float green, float blue, float alpha);
float* graphics_getColorPtr();
void graphics_clear();
void graphics_swap();
void graphics_drawArray(graphics_Quad const* quad, mat4x4 const* tr2d, GLuint vao, GLuint ibo, GLuint count, GLenum type, GLenum indexType, float const * useColor);
int graphics_getWidth();
int graphics_getHeight();
void graphics_setCanvas(graphics_Canvas const* canvas);
void graphics_setColorMask(bool r, bool g, bool b, bool a);
void graphics_getColorMask(bool *r, bool *g, bool *b, bool *a);
graphics_BlendMode graphics_getBlendMode();
void graphics_setBlendMode(graphics_BlendMode mode);
