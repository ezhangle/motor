#include <stdint.h>
#include <SDL.h>
#include "graphics.h"
#include <SDL_opengl.h>


typedef struct {
  unsigned red;
  unsigned green;
  unsigned blue;
  unsigned alpha;
} graphics_Color;

static struct {
  SDL_Surface* surface;
  graphics_Color backgroundColor;
  graphics_Color foregroundColor;
} moduleData;


void graphics_init(int width, int height) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  moduleData.surface = SDL_SetVideoMode(width, height, 32, SDL_OPENGL);
  glViewport(0,0,width,height);
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
