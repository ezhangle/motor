#include "vector.h"


void m4x4_scale(mat4x4 *out, float x, float y, float z) {
  out->m[0][0] = x;
  out->m[0][1] = 0.0f;
  out->m[0][2] = 0.0f;
  out->m[0][3] = 0.0f;
  out->m[1][0] = 0.0f;
  out->m[1][1] = y;
  out->m[1][2] = 0.0f;
  out->m[1][3] = 0.0f;
  out->m[2][0] = 0.0f;
  out->m[2][1] = 0.0f;
  out->m[2][2] = z;
  out->m[2][3] = 0.0f;
  out->m[3][0] = 0.0f;
  out->m[3][1] = 0.0f;
  out->m[3][2] = 0.0f;
  out->m[3][3] = 1.0f;
}

void m4x4_translation(mat4x4 *out, float x, float y, float z) {
  out->m[0][0] = 1.0f;
  out->m[0][1] = 0.0f;
  out->m[0][2] = 0.0f;
  out->m[0][3] = 0.0f;
  out->m[1][0] = 0.0f;
  out->m[1][1] = 1.0f;
  out->m[1][2] = 0.0f;
  out->m[1][3] = 0.0f;
  out->m[2][0] = 0.0f;
  out->m[2][1] = 0.0f;
  out->m[2][2] = 1.0f;
  out->m[2][3] = 0.0f;
  out->m[3][0] = x;
  out->m[3][1] = y;
  out->m[3][2] = z;
  out->m[3][3] = 1.0f;
}

void m4x4_rotation_x(mat4x4 *out, float a) {
}

void m4x4_rotation_y(mat4x4 *out, float a) {
}

void m4x4_rotation_z(mat4x4 *out, float a) {
}

void m4x4_mul_m4x4(mat4x4 *out, mat4x4 const* a, mat4x4 const* b) {
}

void m4x4_mul_v4(vec4 *out, mat4x4 const* m, vec4 const* v) {
}

