#include <stdint.h>
#include <SDL.h>
#include "graphics.h"
#include <SDL_opengl.h>
#include "../math/vector.h"
#include "matrixstack.h"
#include "font.h"
#include "batch.h"
#include "quad.h"

typedef struct {
  float red;
  float green;
  float blue;
  float alpha;
} graphics_Color;

static struct {
  SDL_Surface* surface;
  graphics_Color backgroundColor;
  graphics_Color foregroundColor;
  GLuint imageProgram;
  mat4x4 matrixStack[32];
  mat4x4 projectionMatrix;
  struct {
    GLuint transform;
    GLuint projection;
    GLuint textureRect;
    GLuint tex;
    GLuint color;
  } uniformLocations;

} moduleData;


void graphics_init(int width, int height) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  moduleData.surface = SDL_SetVideoMode(width, height, 32, SDL_OPENGL);
  glViewport(0,0,width,height);

  matrixstack_init();

  m4x4_new_translation(&moduleData.projectionMatrix, -1.0f, 1.0f, 0.0f);
  m4x4_scale(&moduleData.projectionMatrix, 2.0f / width, -2.0f / height, 0.0f);

  moduleData.imageProgram = glCreateProgram();
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

  GLchar const *vsrc = "uniform   mat4 transform;\n"  
                       "uniform   mat4 projection;\n"
                       "uniform   mat2 textureRect;\n"
                       "attribute vec2 vPos;\n"
                       "attribute vec2 vUV;\n"
                       "attribute vec4 vColor;\n"
                       "varying   vec2 fUV;\n"
                       "varying   vec4 fColor;\n"
                       "void main() {\n"
                       "  gl_Position = projection * transform * vec4(vPos, 1.0, 1.0);\n"
                       "  fUV = vUV * textureRect[1] + textureRect[0];\n"
                       "  fColor = vColor;\n"
                       "}\n";

  GLchar const *fsrc = "precision mediump float;\n"
                       "varying vec2 fUV;\n"
                       "varying vec4 fColor;\n"
                       "uniform sampler2D tex;\n"
                       "uniform vec4 color;\n"
                       "void main() {\n"
                       "  gl_FragColor = texture2D(tex, fUV) * color * fColor;\n"
                       "}\n";


  glShaderSource(vertexShader, 1, &vsrc, 0);
  glCompileShader(vertexShader);
  glAttachShader(moduleData.imageProgram, vertexShader);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fsrc, 0);
  glCompileShader(fragmentShader);
  glAttachShader(moduleData.imageProgram, fragmentShader);
  glBindAttribLocation(moduleData.imageProgram, 0, "vPos");
  glBindAttribLocation(moduleData.imageProgram, 1, "vUV");
  glBindAttribLocation(moduleData.imageProgram, 2, "vColor");
  glLinkProgram(moduleData.imageProgram);

  moduleData.uniformLocations.transform = glGetUniformLocation(moduleData.imageProgram, "transform");
  moduleData.uniformLocations.projection = glGetUniformLocation(moduleData.imageProgram, "projection");
  moduleData.uniformLocations.textureRect = glGetUniformLocation(moduleData.imageProgram, "textureRect");
  moduleData.uniformLocations.tex= glGetUniformLocation(moduleData.imageProgram, "tex");
  moduleData.uniformLocations.color= glGetUniformLocation(moduleData.imageProgram, "color");

  graphics_setColor(1.0f, 1.0f, 1.0f, 1.0f);

  graphics_font_init();
  graphics_batch_init();
  graphics_image_init();

  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
}

void graphics_setBackgroundColor(float red, float green, float blue, float alpha) {
  moduleData.backgroundColor.red   = red;
  moduleData.backgroundColor.green = green;
  moduleData.backgroundColor.blue  = blue;
  moduleData.backgroundColor.alpha = alpha;
  glClearColor(red, green, blue, alpha);
}

void graphics_setColor(float red, float green, float blue, float alpha) {
  moduleData.foregroundColor.red   = red;
  moduleData.foregroundColor.green = green;
  moduleData.foregroundColor.blue  = blue;
  moduleData.foregroundColor.alpha = alpha;
}

void graphics_clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void graphics_swap() {
  SDL_GL_SwapBuffers();
}

void graphics_drawArray(graphics_Quad const* quad, mat4x4 const* tr2d, GLuint vao, GLuint ibo, GLuint count, GLenum type, GLenum indexType, float const* useColor) {
  glUseProgram(moduleData.imageProgram);
  glUniform1i(moduleData.uniformLocations.tex, 0);
  // TODO do not request the uniforms every time
  glUniformMatrix4fv(moduleData.uniformLocations.projection, 1, 0, (GLfloat*)&moduleData.projectionMatrix);
  glUniformMatrix2fv(moduleData.uniformLocations.textureRect, 1, 0, (GLfloat*)quad);
  //glUniform4fv(moduleData.uniformLocations.color, 1, (GLfloat*)&moduleData.foregroundColor);
  glUniform4fv(moduleData.uniformLocations.color, 1, useColor);

  mat4x4 tr;
  m4x4_mul_m4x4(&tr, matrixstack_head(), tr2d);

  glUniformMatrix4fv(moduleData.uniformLocations.transform, 1, 0,  (GLfloat*)&tr);
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glDrawElements(type, count, indexType, 0);
}

int graphics_getWidth() {
  return moduleData.surface->w;
}

int graphics_getHeight() {
  return moduleData.surface->h;
}

float* graphics_getColorPtr() {
  return (float*)(&moduleData.foregroundColor);
}
