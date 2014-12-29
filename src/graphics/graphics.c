#include <stdint.h>
#include <SDL.h>
#include "graphics.h"
#include <SDL_opengl.h>
#include "../math/vector.h"
#include "matrixstack.h"


typedef struct {
  unsigned red;
  unsigned green;
  unsigned blue;
  unsigned alpha;
} graphics_Color;

typedef struct {
  float x;
  float y;
  float u;
  float v;
} graphics_Vertex;


static struct {
  SDL_Surface* surface;
  graphics_Color backgroundColor;
  graphics_Color foregroundColor;
  GLuint imageVBO;
  GLuint imageIBO;
  GLuint imageProgram;
  GLuint imageVAO;
  mat4x4 matrixStack[32];
  mat4x4 projectionMatrix;

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
                       "attribute vec2 vPos;\n"
                       "attribute vec2 vUV;\n"
                       "varying   vec2 fUV;\n"
                       "void main() {\n"
                       "  gl_Position = projection * transform * vec4(vPos, 1.0, 1.0);\n"
                       "  fUV = vUV;\n"
                       "}\n";

  GLchar const *fsrc = "precision mediump float;\n"
                       "varying vec2 fUV;\n"
                       "void main() {\n"
                       "  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
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
  glLinkProgram(moduleData.imageProgram);

  GLint len;
  GLint err;
  glGetProgramiv(moduleData.imageProgram, GL_LINK_STATUS, &err);
  printf("compiled: %d\n", err);
  glGetProgramiv(moduleData.imageProgram, GL_INFO_LOG_LENGTH, &len);
  GLchar * infolog = malloc(len);
  glGetProgramInfoLog(moduleData.imageProgram, len, 0, infolog);
  printf("fragment Info Log: %s\n", infolog);


  glGenVertexArrays(1, &moduleData.imageVAO);
  glBindVertexArray(moduleData.imageVAO);
  glGenBuffers(1, &moduleData.imageVBO);
  glGenBuffers(1, &moduleData.imageIBO);

  graphics_Vertex const imageVertices[] = {
    {0.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f}
  };

  unsigned char const imageIndices[] = { 0, 1, 2, 3 };

  glBindBuffer(GL_ARRAY_BUFFER, moduleData.imageVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(imageVertices), imageVertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moduleData.imageIBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(imageIndices), imageIndices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (GLvoid const*)8);
}

void graphics_setBackgroundColor(int red, int green, int blue, int alpha) {
  moduleData.backgroundColor.red   = red;
  moduleData.backgroundColor.green = green;
  moduleData.backgroundColor.blue  = blue;
  moduleData.backgroundColor.alpha = alpha;
  float scale = 1.0f / 255.0f;
  glClearColor(red * scale, green * scale, blue * scale, alpha * scale);
}

void graphics_setColor(int red, int green, int blue, int alpha) {
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

void graphics_draw_Image(graphics_Image* image) {
  glUseProgram(moduleData.imageProgram);


  glUniformMatrix4fv(glGetUniformLocation(moduleData.imageProgram, "projection"), 1, 0, &moduleData.projectionMatrix);
  glUniformMatrix4fv(glGetUniformLocation(moduleData.imageProgram, "transform"), 1, 0,  matrixstack_head());

  glBindVertexArray(moduleData.imageVAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moduleData.imageIBO);
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, 0);

}
