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


void m4x4_new_identity(mat4x4 *out);
void m4x4_new_scaling(mat4x4 *out, float x, float y, float z);
void m4x4_new_translation(mat4x4 *out, float x, float y, float z);
void m4x4_new_rotation_x(mat4x4 *out, float a);
void m4x4_new_rotation_y(mat4x4 *out, float a);
void m4x4_new_rotation_z(mat4x4 *out, float a);
void m4x4_new_transform2d(mat4x4 *out, float x, float y, float r, float sx, float sy,
                      float ox, float oy, float kx, float ky, float w, float h);
void m4x4_mul_m4x4(mat4x4 *out, mat4x4 const* a, mat4x4 const* b);
void m4x4_mul_v4(vec4 *out, mat4x4 const* m, vec4 const* v);
void m4x4_scale(mat4x4 *inout, float x, float y, float z);
void m4x4_translate(mat4x4 *inout, float x, float y, float z);
void m4x4_rotate_z(mat4x4 *inout, float a);
void m4x4_shear2d(mat4x4 *inout, float x, float y);
