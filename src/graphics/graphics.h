#pragma once

#include "image.h"
#include "quad.h"

void graphics_init(int width, int height);

void graphics_setBackgroundColor(int red, int green, int blue, int alpha);
void graphics_setColor(int red, int green, int blue, int alpha);
void graphics_clear();
void graphics_swap();
void graphics_draw_Image(graphics_Image const* image, graphics_Quad const* quad,
                         float x, float y, float r, float sx, float sy,
                         float ox, float oy, float kx, float ky);
