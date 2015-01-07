#include <stdint.h>
#include <SDL.h>
#include "graphics.h"
#include <SDL_opengl.h>
#include "../math/vector.h"
#include "matrixstack.h"
#include "font.h"


typedef struct {
  float red;
  float green;
  float blue;
  float alpha;
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
                       "uniform   mat2 textureRect;\n"
                       "attribute vec2 vPos;\n"
                       "attribute vec2 vUV;\n"
                       "varying   vec2 fUV;\n"
                       "void main() {\n"
                       "  gl_Position = projection * transform * vec4(vPos, 1.0, 1.0);\n"
                       "  fUV = vUV * textureRect[1] + textureRect[0];\n"
                       "}\n";

  GLchar const *fsrc = "precision mediump float;\n"
                       "varying vec2 fUV;\n"
                       "uniform sampler2D tex;\n"
                       "uniform vec4 color;\n"
                       "void main() {\n"
                       "  gl_FragColor = texture2D(tex, fUV) * color;\n"
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

  graphics_setColor(1.0f, 1.0f, 1.0f, 1.0f);

  graphics_font_init();

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

void graphics_draw_Image(graphics_Image const* image, graphics_Quad const* quad,
                         float x, float y, float r, float sx, float sy,
                         float ox, float oy, float kx, float ky) {
  glUseProgram(moduleData.imageProgram);
//  printf("Program: %d\n", moduleData.imageProgram);


  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, image->texID);
  glUniform1i(glGetUniformLocation(moduleData.imageProgram, "tex"), 0);
  glUniformMatrix4fv(glGetUniformLocation(moduleData.imageProgram, "projection"), 1, 0, (GLfloat*)&moduleData.projectionMatrix);
  glUniformMatrix2fv(glGetUniformLocation(moduleData.imageProgram, "textureRect"), 1, 0, (GLfloat*)quad);
  glUniform4fv(glGetUniformLocation(moduleData.imageProgram, "color"), 1, (GLfloat*)&moduleData.foregroundColor);

  mat4x4 tr;
  // TODO add m4x4_transform2d and use it. that would safe a lot of multiplications and additions
  mat4x4 tr2d;
  m4x4_new_transform2d(&tr2d, x, y, r, sx, sy, ox, oy, kx, ky, image->width * quad->w, image->height * quad->h);
  m4x4_mul_m4x4(&tr, matrixstack_head(), &tr2d);

  glUniformMatrix4fv(glGetUniformLocation(moduleData.imageProgram, "transform"), 1, 0,  (GLfloat*)&tr);

  glBindVertexArray(moduleData.imageVAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moduleData.imageIBO);
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, 0);

}
