#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "shader.h"
#include "../3rdparty/slre/slre.h"

static struct {
  graphics_Shader *activeShader;
  graphics_Shader defaultShader;
  int maxTextureUnits;
} moduleData;

GLchar const *defaultVertexSource = 
  "vec4 position(mat4 transform_projection, vec4 vertex_position) {\n"
  "  return transform_projection * vertex_position;\n"
  "}\n";

static GLchar const vertexHeader[] =
  "uniform   mat4 transform;\n"  
  "uniform   mat4 projection;\n"
  "uniform   mat2 textureRect;\n"
  "uniform   vec2 size;\n"
  "#define extern uniform\n"
  "attribute vec2 vPos;\n"
  "attribute vec2 vUV;\n"
  "attribute vec4 vColor;\n"
  "varying   vec2 fUV;\n"
  "varying   vec4 fColor;\n"
  "#line 0\n";

static GLchar const vertexFooter[] =
  "void main() {\n"
  "  gl_Position = position(projection * transform, vec4(vPos * size, 1.0, 1.0));\n"
  "  fUV = vUV * textureRect[1] + textureRect[0];\n"
  "  fColor = vColor;\n"
  "}\n";

static GLchar const *defaultFragmentSource =
  "vec4 effect( vec4 color, Image texture, vec2 texture_coords, vec2 screen_coords ) {\n"
  "  return Texel(texture, texture_coords) * color;\n"
  "}\n";

#define DEFAULT_SAMPLER "tex"

static GLchar const fragmentHeader[] = 
  "precision mediump float;\n"
  "#define Image sampler2D\n"
  "#define Texel texture2D\n"
  "#define extern uniform\n"
  "varying vec2 fUV;\n"
  "varying vec4 fColor;\n"
  "uniform sampler2D " DEFAULT_SAMPLER ";\n"
  "uniform vec4 color;\n"
  "#line 0\n";

static GLchar const fragmentFooter[] =
  "void main() {\n"
  "  gl_FragColor = effect(color * fColor, tex, fUV, vec2(0.0, 0.0));\n"
  "}\n";

void graphics_Shader_compileAndAttachShaderRaw(GLuint program, GLenum shaderType, char const* code) {
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, (GLchar const **)&code, 0);
  glCompileShader(shader);

  glAttachShader(program, shader);
  glDeleteShader(shader);
}

void graphics_Shader_compileAndAttachShader(GLuint program, GLenum shaderType, char const* code) {
  GLchar const* header;
  GLchar const* footer;
  int headerlen;
  int footerlen;
  switch(shaderType) {
  case GL_VERTEX_SHADER:
    header = vertexHeader;
    headerlen = sizeof(vertexHeader) - 1;
    footer = vertexFooter;
    footerlen = sizeof(vertexFooter) - 1;
    break;
  case GL_FRAGMENT_SHADER:
    header = fragmentHeader;
    headerlen = sizeof(fragmentHeader) - 1;
    footer = fragmentFooter;
    footerlen = sizeof(fragmentFooter) - 1;
    break;
  }
  int codelen = strlen(code);
  GLchar *combinedCode = malloc(headerlen + footerlen + codelen + 1);
  memcpy(combinedCode, header, headerlen);
  memcpy(combinedCode + headerlen, (GLchar const*)code, codelen);
  memcpy(combinedCode + headerlen + codelen, footer, footerlen+1); // include zero terminator

  graphics_Shader_compileAndAttachShaderRaw(program, shaderType, combinedCode);

  free(combinedCode);
}

static int compareUniformInfo(graphics_ShaderUniformInfo const* a, graphics_ShaderUniformInfo const* b) {
  return strcmp(a->name, b->name);
}

int graphics_shader_toMotorComponents(GLenum type) {
  switch(type) {
  case GL_BOOL:
  case GL_INT:
  case GL_FLOAT:
    return 1;

  case GL_BOOL_VEC2:
  case GL_INT_VEC2:
  case GL_FLOAT_VEC2:
  case GL_FLOAT_MAT2:
    return 2;

  case GL_BOOL_VEC3:
  case GL_INT_VEC3:
  case GL_FLOAT_VEC3:
  case GL_FLOAT_MAT3:
    return 3;


  case GL_BOOL_VEC4:
  case GL_INT_VEC4:
  case GL_FLOAT_VEC4:
  case GL_FLOAT_MAT4:
    return 4;

  default:
    return 0;
  };
}

graphics_ShaderUniformType graphics_shader_toMotorType(GLenum type) {
  switch(type) {
  case GL_BOOL:
  case GL_BOOL_VEC2:
  case GL_BOOL_VEC3:
  case GL_BOOL_VEC4:
    return graphics_ShaderUniformType_bool;

  case GL_INT:
  case GL_INT_VEC2:
  case GL_INT_VEC3:
  case GL_INT_VEC4:
    return graphics_ShaderUniformType_int;

  case GL_FLOAT:
  case GL_FLOAT_VEC2:
  case GL_FLOAT_VEC3:
  case GL_FLOAT_VEC4:
  case GL_FLOAT_MAT2:
  case GL_FLOAT_MAT3:
  case GL_FLOAT_MAT4:
    return graphics_ShaderUniformType_float;

  case GL_SAMPLER_2D:
    return graphics_ShaderUniformType_sampler;

  default:
    return graphics_ShaderUniformType_none;
  };
}

static void readShaderUniforms(graphics_Shader *shader) {
  shader->uniformLocations.transform   = glGetUniformLocation(shader->program, "transform");
  shader->uniformLocations.projection  = glGetUniformLocation(shader->program, "projection");
  shader->uniformLocations.textureRect = glGetUniformLocation(shader->program, "textureRect");
  shader->uniformLocations.tex         = glGetUniformLocation(shader->program, DEFAULT_SAMPLER);
  shader->uniformLocations.color       = glGetUniformLocation(shader->program, "color");
  shader->uniformLocations.size        = glGetUniformLocation(shader->program, "size");

  int maxLength;
  glGetProgramiv(shader->program, GL_ACTIVE_UNIFORMS, &shader->uniformCount);
  glGetProgramiv(shader->program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

  // This wastes some memory, but it's also relatively fast
  shader->uniforms = (graphics_ShaderUniformInfo*)malloc(sizeof(graphics_ShaderUniformInfo) * shader->uniformCount);
  for(int i = 0; i < shader->uniformCount; ++i) {
    graphics_ShaderUniformInfo* info = shader->uniforms+i;
    info->name = (char*)malloc(maxLength);
    glGetActiveUniform(shader->program, i, maxLength, NULL, &info->elements, &info->type, info->name);

    info->location = glGetUniformLocation(shader->program, info->name);
    info->extra = 0;

    char *suffix = strstr(info->name, "[0]");
    if(suffix) {
      *suffix = 0;
    }
  }

  qsort(shader->uniforms, shader->uniformCount, sizeof(graphics_ShaderUniformInfo), (int(*)(void const*,void const*))compareUniformInfo);
}


static void allocateTextureUnits(graphics_Shader *shader) {
  shader->textureUnitCount = 0;
  for(int i = 0; i < shader->uniformCount; ++i) {
    if(shader->uniforms[i].type == GL_SAMPLER_2D) {
      if(strcmp(shader->uniforms[i].name, DEFAULT_SAMPLER)) {
        ++shader->textureUnitCount;
      }
    }
  }  

  shader->textureUnits = malloc(sizeof(graphics_ShaderTextureUnitInfo) * shader->textureUnitCount);

  int currentUnit = 0;
  glUseProgram(shader->program);
  for(int i = 0; i < shader->uniformCount; ++i) {
    if(shader->uniforms[i].type == GL_SAMPLER_2D) {
      if(strcmp(shader->uniforms[i].name, DEFAULT_SAMPLER)) {
        glUniform1i(shader->uniforms[i].location, currentUnit+1);
        shader->uniforms[i].extra = shader->textureUnits + currentUnit;
        shader->textureUnits[currentUnit].unit = currentUnit + 1;
        shader->textureUnits[currentUnit].boundTexture = 0;
        ++currentUnit;
      }
    }
  }
}


void graphics_Shader_new(graphics_Shader *shader, char const* vertexCode, char const* fragmentCode) {
  if(!vertexCode) {
    vertexCode = defaultVertexSource;
  }

  if(!fragmentCode) {
    fragmentCode = defaultFragmentSource;
  }

  shader->program = glCreateProgram();

  graphics_Shader_compileAndAttachShader(shader->program, GL_VERTEX_SHADER, vertexCode);
  graphics_Shader_compileAndAttachShader(shader->program, GL_FRAGMENT_SHADER, fragmentCode);

  glBindAttribLocation(shader->program, 0, "vPos");
  glBindAttribLocation(shader->program, 1, "vUV");
  glBindAttribLocation(shader->program, 2, "vColor");
  glLinkProgram(shader->program);

  readShaderUniforms(shader);

  allocateTextureUnits(shader);
}


void graphics_Shader_free(graphics_Shader* shader) {
  glDeleteProgram(shader->program);

  for(int i = 0; i < shader->uniformCount; ++i) {
    free(shader->uniforms[i].name);
  }
  free(shader->textureUnits);
  free(shader->uniforms);
}

void graphics_Shader_activate(mat4x4 const* projection, mat4x4 const* transform, graphics_Quad const* textureRect, float const* useColor, float ws, float hs) {

  glUseProgram(moduleData.activeShader->program);

  float s[2] = { ws, hs };

  glUniform1i(       moduleData.activeShader->uniformLocations.tex,               0);
  glUniformMatrix4fv(moduleData.activeShader->uniformLocations.projection,  1, 0, (GLfloat const*)projection);
  glUniformMatrix2fv(moduleData.activeShader->uniformLocations.textureRect, 1, 0, (GLfloat const*)textureRect);
  glUniform4fv(      moduleData.activeShader->uniformLocations.color,       1,                    useColor);
  glUniform2fv(      moduleData.activeShader->uniformLocations.size,        1,                    s);
  glUniformMatrix4fv(moduleData.activeShader->uniformLocations.transform,   1, 0, (GLfloat const*)transform);

  for(int i = 0; i < moduleData.activeShader->textureUnitCount; ++i) {
    glActiveTexture(GL_TEXTURE0 + moduleData.activeShader->textureUnits[i].unit);
    glBindTexture(GL_TEXTURE_2D, moduleData.activeShader->textureUnits[i].boundTexture);
  }
}

void graphics_setDefaultShader() {
  moduleData.activeShader = &moduleData.defaultShader;
}

void graphics_setShader(graphics_Shader* shader) {
  moduleData.activeShader = shader;
}

graphics_Shader* graphics_getShader() {
  return moduleData.activeShader;
}

void graphics_shader_init() {
  graphics_Shader_new(&moduleData.defaultShader, NULL, NULL);
  moduleData.activeShader = &moduleData.defaultShader;
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &moduleData.maxTextureUnits);
}

#define mkScalarSendFunc(name, type, glfunc) \
  void graphics_Shader_ ## name(graphics_Shader *shader, graphics_ShaderUniformInfo const* info, int count, type const* numbers) {  \
    glUseProgram(shader->program); \
    glfunc(info->location, count, numbers); \
  }

mkScalarSendFunc(sendIntegers, GLint,   glUniform1iv)
mkScalarSendFunc(sendBooleans, GLint,   glUniform1iv)
mkScalarSendFunc(sendFloats,   GLfloat, glUniform1fv)

#undef mkScalarSendFunc

#define mkVectorSendFunc(name, valuetype, abbr) \
  void graphics_Shader_ ## name(graphics_Shader *shader, graphics_ShaderUniformInfo const* info, int count, valuetype const* numbers) {  \
    glUseProgram(shader->program);                                \
    switch(graphics_shader_toMotorComponents(info->type)) {       \
    case 2:                                                       \
      glUniform2 ## abbr ## v(info->location, count, numbers);    \
      break;                                                      \
    case 3:                                                       \
      glUniform3 ## abbr ## v(info->location, count, numbers);    \
      break;                                                      \
    case 4:                                                       \
      glUniform4 ## abbr ## v(info->location, count, numbers);    \
      break;                                                      \
    }                                                             \
  }

mkVectorSendFunc(sendIntegerVectors, GLint,   i)
mkVectorSendFunc(sendBooleanVectors, GLint,   i)
mkVectorSendFunc(sendFloatVectors,   GLfloat, f)

#undef mkVectorSendFunc

void graphics_Shader_sendFloatMatrices(graphics_Shader *shader, graphics_ShaderUniformInfo const* info, int count, float const* numbers) {
  glUseProgram(shader->program);

  switch(graphics_shader_toMotorComponents(info->type)) {
  case 2:
    glUniformMatrix2fv(info->location, count, false, numbers);
    break;

  case 3:
    glUniformMatrix3fv(info->location, count, false, numbers);
    break;

  case 4:
    glUniformMatrix4fv(info->location, count, false, numbers);
    break;
  }
}


void graphics_Shader_sendTexture(graphics_Shader *shader, graphics_ShaderUniformInfo const* info, GLuint texture) {
  graphics_ShaderTextureUnitInfo *unit = (graphics_ShaderTextureUnitInfo*)info->extra;
  unit->boundTexture = texture;
}


graphics_ShaderUniformInfo const* graphics_Shader_getUniform(graphics_Shader const* shader, char const* name) {
  // Dirty trick to avoid duplicate code: Name will be treated as graphics_ShaderUniformInfo.
  return bsearch(&name, shader->uniforms, shader->uniformCount, sizeof(graphics_ShaderUniformInfo), (int(*)(void const*, void const*))compareUniformInfo);
}
