#include <SDL.h>
#include <stdio.h>

int main()
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  SDL_CreateWindowAndRenderer(600, 600, SDL_WINDOW_OPENGL, &window, &renderer);

}
