#pragma once

#include "graphics.h"

void graphics_geometry_init();

void graphics_geometry_fill_rectangle(float x, float y, float w, float h);
void graphics_geometry_draw_rectangle(float x, float y, float w, float h);

void graphics_geometry_fill_circle(float x, float y, float radius, int segments);
void graphics_geometry_draw_circle(float x, float y, float radius, int segments);

float graphics_geometry_get_line_width();
void  graphics_geometry_set_line_width(float width);
