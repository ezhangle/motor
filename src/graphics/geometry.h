#pragma once

#include "graphics.h"

typedef enum {
  graphics_DrawMode_fill,
  graphics_DrawMode_line
} graphics_DrawMode;

typedef enum {
  graphics_LineJoin_none,
  graphics_LineJoin_miter,
  graphics_LineJoin_bevel
} graphics_LineJoin;

void graphics_geometry_init();

void graphics_geometry_fill_rectangle(float x, float y, float w, float h);
void graphics_geometry_draw_rectangle(float x, float y, float w, float h);

void graphics_geometry_fill_circle(float x, float y, float radius, int segments);
void graphics_geometry_draw_circle(float x, float y, float radius, int segments);

void graphics_geometry_draw_lines(int vertexCount, float const* vertices);

void graphics_geometry_set_line_join(graphics_LineJoin join);
graphics_LineJoin graphics_geometry_get_line_join();

float graphics_geometry_get_line_width();
void  graphics_geometry_set_line_width(float width);
