#pragma once

#include <SDL_opengl.h>
#include "../math/vector.h"
#include "quad.h"

typedef enum {
  graphics_ShaderUniformType_float,
  graphics_ShaderUniformType_int,
  graphics_ShaderUniformType_bool,
  graphics_ShaderUniformType_sampler,
  graphics_ShaderUniformType_none
} graphics_ShaderUniformType;

typedef struct {
  char *name;
  GLenum type;
  int elements;
  GLint location;
} graphics_ShaderUniformInfo;

typedef struct {
  // These are regularly needed on a per-drawcall basis.
  // This way we can access them real quick
  struct {
    GLuint transform;
    GLuint projection;
    GLuint textureRect;
    GLuint tex;
    GLuint color;
    GLuint size;
  } uniformLocations;

  int uniformCount;
  graphics_ShaderUniformInfo *uniforms;

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
graphics_ShaderUniformInfo const* graphics_Shader_getUniform(graphics_Shader const* shader, char const* name);
graphics_ShaderUniformType graphics_shader_toMotorType(GLenum type);
int graphics_shader_toMotorComponents(GLenum type);

void graphics_Shader_sendIntegers(graphics_Shader *shader, graphics_ShaderUniformInfo const* info, int count, GLint const* numbers);
void graphics_Shader_sendBooleans(graphics_Shader *shader, graphics_ShaderUniformInfo const* info, int count, GLint const* numbers);
void graphics_Shader_sendFloats(graphics_Shader *shader, graphics_ShaderUniformInfo const* info, int count, GLfloat const* numbers);
void graphics_Shader_sendIntegerVectors(graphics_Shader *shader, graphics_ShaderUniformInfo const* info, int count, GLint const* numbers);
void graphics_Shader_sendFloatVectors(graphics_Shader *shader, graphics_ShaderUniformInfo const* info, int count, GLfloat const* numbers);
void graphics_Shader_sendBooleanVectors(graphics_Shader *shader, graphics_ShaderUniformInfo const* info, int count, GLint const* numbers);
