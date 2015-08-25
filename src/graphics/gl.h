#pragma once

#define NO_SDL_GLEXT 
#include <GL/glew.h> 
#include <GL/gl.h>
#ifdef SDL2_USED
#include <SDL.h>
#include <SDL_opengl.h>
#else
#include <SDL/SDL.h> 
#include <SDL/SDL_opengl.h>
#endif
