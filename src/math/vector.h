#pragma once

typedef struct {
  float x;
  float y;
  float z;
  float w;
} vec4;


typedef struct {
  float m[4][4];
} mat4x4;


void m4x4_scale(mat4x4 *out, float x, float y, float z);
void m4x4_translation(mat4x4 *out, float x, float y, float z);
void m4x4_rotation_x(mat4x4 *out, float a);
void m4x4_rotation_y(mat4x4 *out, float a);
void m4x4_rotation_z(mat4x4 *out, float a);
void m4x4_mul_m4x4(mat4x4 *out, mat4x4 const* a, mat4x4 const* b);
void m4x4_mul_v4(vec4 *out, mat4x4 const* m, vec4 const* v);
