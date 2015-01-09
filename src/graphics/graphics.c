#include <stdint.h>
#include <SDL.h>
#include "graphics.h"
#include <SDL_opengl.h>
#include "../math/vector.h"
#include "matrixstack.h"
#include "font.h"
#include "batch.h"
#include "quad.h"
#include "canvas.h"

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
  //mat4x4 projectionMatrix;
  struct {
    GLuint transform;
    GLuint projection;
    GLuint textureRect;
    GLuint tex;
    GLuint color;
  } uniformLocations;

  graphics_Canvas const * canvas;
  graphics_Canvas defaultCanvas;
  bool colorMask[4];
  graphics_BlendMode blendMode;
  int scissorBox[4];
  bool scissorSet;
  
} moduleData;


void graphics_init(int width, int height) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  moduleData.surface = SDL_SetVideoMode(width, height, 32, SDL_OPENGL);
  glViewport(0,0,width,height);

  matrixstack_init();

  m4x4_new_translation(&moduleData.defaultCanvas.projectionMatrix, -1.0f, 1.0f, 0.0f);
  m4x4_scale(&moduleData.defaultCanvas.projectionMatrix, 2.0f / width, -2.0f / height, 0.0f);
  moduleData.defaultCanvas.fbo = 0;
  moduleData.defaultCanvas.width = width;
  moduleData.defaultCanvas.height = height;
  moduleData.canvas = &moduleData.defaultCanvas;

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

  graphics_setColorMask(true, true, true, true);
  graphics_setBlendMode(graphics_BlendMode_alpha);
  graphics_clearScissor();
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
  glUniformMatrix4fv(moduleData.uniformLocations.projection, 1, 0, (GLfloat*)&moduleData.canvas->projectionMatrix);
  glUniformMatrix2fv(moduleData.uniformLocations.textureRect, 1, 0, (GLfloat*)quad);
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

void graphics_setCanvas(graphics_Canvas const* canvas) {
  if(!canvas) {
    canvas = &moduleData.defaultCanvas;
  }
  moduleData.canvas = canvas;
  glBindFramebuffer(GL_FRAMEBUFFER, canvas->fbo);

  glViewport(0,0,canvas->width, canvas->height);
}

void graphics_setColorMask(bool r, bool g, bool b, bool a) {
  moduleData.colorMask[0] = r;
  moduleData.colorMask[1] = g;
  moduleData.colorMask[2] = b;
  moduleData.colorMask[3] = a;

  glColorMask(r,g,b,a);
}

void graphics_getColorMask(bool *r, bool *g, bool *b, bool *a) {
  *r = moduleData.colorMask[0];
  *g = moduleData.colorMask[1];
  *b = moduleData.colorMask[2];
  *a = moduleData.colorMask[3];
}

graphics_BlendMode graphics_getBlendMode() {
  return moduleData.blendMode;
}

void graphics_setBlendMode(graphics_BlendMode mode) {
  moduleData.blendMode = mode;

  GLenum sfRGB = GL_ONE;
  GLenum dfRGB = GL_ZERO;
  GLenum sfA = GL_ONE;
  GLenum dfA = GL_ZERO;
  GLenum bFunc = GL_FUNC_ADD;

  switch(mode) {
  case graphics_BlendMode_alpha:
    sfRGB = GL_SRC_ALPHA;
    sfA = GL_ONE;
    dfRGB = dfA = GL_ONE_MINUS_SRC_ALPHA;
    break;

  case graphics_BlendMode_subtractive:
    bFunc = GL_FUNC_REVERSE_SUBTRACT;
    // fallthrough
  case graphics_BlendMode_additive:
    sfA = sfRGB = GL_SRC_ALPHA;
    dfA = dfRGB = GL_ONE;
    break;


  case graphics_BlendMode_multiplicative:
    sfA = sfRGB = GL_DST_COLOR;
    dfA = dfRGB = GL_ZERO;
    break;

  case graphics_BlendMode_premultiplied:
    sfA = sfRGB = GL_ONE;
    dfA = dfRGB = GL_ONE_MINUS_SRC_ALPHA;
    break;

  case graphics_BlendMode_screen:
    sfA = sfRGB = GL_ONE;
    dfA = dfRGB = GL_ONE_MINUS_SRC_COLOR;
    break;

  case graphics_BlendMode_replace:
  default:
    // uses default init values   
    break;
  }

  glBlendFuncSeparate(sfRGB, dfRGB, sfA, dfA);
  glBlendEquation(bFunc);
}

void graphics_clearScissor() {
  moduleData.scissorSet = false;
  glDisable(GL_SCISSOR_TEST);
}

void graphics_setScissor(int x, int y, int w, int h) {
  moduleData.scissorBox[0] = x;
  moduleData.scissorBox[1] = y;
  moduleData.scissorBox[2] = w;
  moduleData.scissorBox[3] = h;
  moduleData.scissorSet = true;
  glScissor(x,y,w,h);
  glEnable(GL_SCISSOR_TEST);
}

bool graphics_getScissor(int *x, int *y, int *w, int *h) {
  if(!moduleData.scissorSet) {
    return false;
  }

  *x = moduleData.scissorBox[0];
  *y = moduleData.scissorBox[1];
  *w = moduleData.scissorBox[2];
  *h = moduleData.scissorBox[3];

  return true;
}
