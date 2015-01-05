#pragma once

#include <SDL/SDL_ttf.h>

typedef struct {
  TTF_Font *font;
} graphics_Font;

void graphics_font_init();
int graphics_Font_new(graphics_Font *dst, char const* filename, int ptsize);
void graphics_Font_free(graphics_Font *obj);
int graphics_Font_getHeight(graphics_Font const* font); 
int graphics_Font_getAscent(graphics_Font const* font);
int graphics_Font_getDescent(graphics_Font const* font);
int graphics_Font_getBaseline(graphics_Font const* font);
int graphics_Font_getWidth(graphics_Font const* font, char const* line);
int graphics_Font_getWrap(graphics_Font const* font, char const* line, int width, char ** wrapped);
