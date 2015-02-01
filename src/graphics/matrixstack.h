#pragma once

#include "../math/vector.h"

void matrixstack_init();
int matrixstack_push();
int matrixstack_pop();
mat4x4 * matrixstack_head();
void matrixstack_translate(float x, float y);
void matrixstack_scale(float x, float y);
void matrixstack_origin();
void matrixstack_rotate(float a);
void matrixstack_multiply(mat4x4 const* matrix);
