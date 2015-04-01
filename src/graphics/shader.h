#pragma once

#include <SDL_opengl.h>
#include "../math/vector.h"
#include "quad.h"

typedef struct {
  struct {
    GLuint transform;
    GLuint projection;
    GLuint textureRect;
    GLuint tex;
    GLuint color;
    GLuint size;
  } uniformLocations;

  GLuint program;
} graphics_Shader;

void graphics_Shader_new(graphics_Shader *shader, char const* vertexCode, char const* fragmentCode);
void graphics_Shader_activate(mat4x4 const* projection, mat4x4 const* transform, graphics_Quad const* textureRect, float const* useColor, float ws,float hs);
graphics_Shader* graphics_getShader();
void graphics_shader_init();
void graphics_Shader_free(graphics_Shader* shader);
void graphics_setDefaultShader();
void graphics_setShader(graphics_Shader* shader);
void graphics_Shader_compileAndAttachShaderRaw(GLuint program, GLenum shaderType, char const* code);
void graphics_Shader_compileAndAttachShader(GLuint program, GLenum shaderType, char const* code);
