#include <stdlib.h>
#include <string.h>
#include "shader.h"
#include "../3rdparty/slre/slre.h"

static struct {
  graphics_Shader *activeShader;
  graphics_Shader defaultShader;
} moduleData;

GLchar const *defaultVertexSource = 
  "vec4 position(mat4 transform_projection, vec4 vertex_position) {\n"
  "  return transform_projection * vertex_position;\n"
  "}\n";

static GLchar const vertexHeader[] =
  "uniform   mat4 transform;\n"  
  "uniform   mat4 projection;\n"
  "uniform   mat2 textureRect;\n"
  "attribute vec2 vPos;\n"
  "attribute vec2 vUV;\n"
  "attribute vec4 vColor;\n"
  "varying   vec2 fUV;\n"
  "varying   vec4 fColor;\n";

static GLchar const vertexFooter[] =
  "void main() {\n"
  "  gl_Position = position(projection * transform, vec4(vPos, 1.0, 1.0));\n"
  "  fUV = vUV * textureRect[1] + textureRect[0];\n"
  "  fColor = vColor;\n"
  "}\n";

static GLchar const *defaultFragmentSource =
  "vec4 effect( vec4 color, Image texture, vec2 texture_coords, vec2 screen_coords ) {\n"
  "  return Texel(texture, texture_coords) * color;\n"
  "}\n";

static GLchar const fragmentHeader[] = 
  "precision mediump float;\n"
  "#define Image sampler2D\n"
  "#define Texel texture2D\n"
  "varying vec2 fUV;\n"
  "varying vec4 fColor;\n"
  "uniform sampler2D tex;\n"
  "uniform vec4 color;\n";

static GLchar const fragmentFooter[] =
  "void main() {\n"
  "  gl_FragColor = effect(color * fColor, tex, fUV, vec2(0.0, 0.0));\n"
  "}\n";


static void compileAndAttachShader(GLuint program, GLenum shaderType, char const* code) {
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

  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &combinedCode, 0);
  glCompileShader(shader);

  glAttachShader(program, shader);
  glDeleteShader(shader);

  free(combinedCode);
}

void graphics_Shader_new(graphics_Shader *shader, char const* vertexCode, char const* fragmentCode) {
  if(!vertexCode) {
    vertexCode = defaultVertexSource;
  }

  if(!fragmentCode) {
    fragmentCode = defaultFragmentSource;
  }

  shader->program = glCreateProgram();
  /*
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexCode, 0);
  glCompileShader(vertexShader);
  glAttachShader(shader->program, vertexShader);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentCode, 0);
  glCompileShader(fragmentShader);
  glAttachShader(shader->program, fragmentShader);
  */

  compileAndAttachShader(shader->program, GL_VERTEX_SHADER, vertexCode);
  compileAndAttachShader(shader->program, GL_FRAGMENT_SHADER, fragmentCode);

  glBindAttribLocation(shader->program, 0, "vPos");
  glBindAttribLocation(shader->program, 1, "vUV");
  glBindAttribLocation(shader->program, 2, "vColor");
  glLinkProgram(shader->program);

  shader->uniformLocations.transform   = glGetUniformLocation(shader->program, "transform");
  shader->uniformLocations.projection  = glGetUniformLocation(shader->program, "projection");
  shader->uniformLocations.textureRect = glGetUniformLocation(shader->program, "textureRect");
  shader->uniformLocations.tex         = glGetUniformLocation(shader->program, "tex");
  shader->uniformLocations.color       = glGetUniformLocation(shader->program, "color");
}

void graphics_Shader_free(graphics_Shader* shader) {
  // TODO
}

void graphics_Shader_activate(mat4x4 const* projection, mat4x4 const* transform, graphics_Quad const* textureRect, float const* useColor) {

  glUseProgram(moduleData.activeShader->program);
  //printf("Using shader %d\n", moduleData.activeShader->program);

  glUniform1i(       moduleData.activeShader->uniformLocations.tex,               0);
  glUniformMatrix4fv(moduleData.activeShader->uniformLocations.projection,  1, 0, (GLfloat const*)projection);
  glUniformMatrix2fv(moduleData.activeShader->uniformLocations.textureRect, 1, 0, (GLfloat const*)textureRect);
  glUniform4fv(      moduleData.activeShader->uniformLocations.color,       1,                    useColor);
  glUniformMatrix4fv(moduleData.activeShader->uniformLocations.transform,   1, 0, (GLfloat const*)transform);
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
}
