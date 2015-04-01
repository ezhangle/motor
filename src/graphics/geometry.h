#pragma once

#include "graphics.h"

void graphics_geometry_init();

void graphics_geometry_fill_rectangle(float x, float y, float w, float h);

void graphics_geometry_fill_circle(float x, float y, float radius, int segments);
