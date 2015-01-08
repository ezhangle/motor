#pragma once

#include "image.h"
#include "quad.h"
#include "../math/vector.h"


void graphics_init(int width, int height);

void graphics_setBackgroundColor(float red, float green, float blue, float alpha);
void graphics_setColor(float red, float green, float blue, float alpha);
void graphics_clear();
void graphics_swap();
void graphics_drawArray(graphics_Quad const* quad, mat4x4 const* tr2d, GLuint vao, GLuint ibo, GLuint count, GLenum type, GLenum indexType);
int graphics_getWidth();
int graphics_getHeight();
