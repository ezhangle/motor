#include <stdint.h>
#include <SDL.h>
#include "graphics.h"
#include <SDL_opengl.h>


typedef struct {
  unsigned red;
  unsigned green;
  unsigned blue;
  unsigned alpha;
} motor_graphics_Color;

typedef struct {
  SDL_Surface* surface;
  motor_graphics_Color backgroundColor;
  motor_graphics_Color foregroundColor;
} motor_graphics_ModuleData;

static motor_graphics_ModuleData moduleData;

void motor_graphics_init(int width, int height) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  moduleData.surface = SDL_SetVideoMode(width, height, 32, SDL_OPENGL);
  glViewport(0,0,width,height);
}

void motor_graphics_setBackgroundColor(int red, int green, int blue, int alpha) {
  moduleData.backgroundColor.red   = red;
  moduleData.backgroundColor.green = green;
  moduleData.backgroundColor.blue  = blue;
  moduleData.backgroundColor.alpha = alpha;
  float scale = 1.0f / 255.0f;
  glClearColor(red * scale, green * scale, blue * scale, alpha * scale);
}

void motor_graphics_setColor(int red, int green, int blue, int alpha) {
  moduleData.foregroundColor.red   = red;
  moduleData.foregroundColor.green = green;
  moduleData.foregroundColor.blue  = blue;
  moduleData.foregroundColor.alpha = alpha;
}

void motor_graphics_clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void motor_graphics_swap() {
  SDL_GL_SwapBuffers();
}
