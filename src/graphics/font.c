#include <tgmath.h>
#include "font.h"

void graphics_font_init() {
  TTF_Init();
}

int graphics_Font_new(graphics_Font *dst, char const* filename, int ptsize) {
  dst->font = TTF_OpenFont(filename, ptsize);
  return dst->font == NULL;
}

void graphics_Font_free(graphics_Font *obj) {
  TTF_CloseFont(obj->font);
}

int graphics_Font_getHeight(graphics_Font const* font) {
  return TTF_FontHeight(font->font);
}

int graphics_Font_getAscent(graphics_Font const* font) {
  return TTF_FontAscent(font->font);
}

int graphics_Font_getDescent(graphics_Font const* font) {
  return TTF_FontDescent(font->font);
}

int graphics_Font_getBaseline(graphics_Font const* font) {
  // No idea how this works, I stole it from Love2d
  return floor(graphics_Font_getHeight(font) / 1.25f + 0.5f);
}

int graphics_Font_getWidth(graphics_Font const* font, char const* line) {
  int w;
  TTF_SizeUTF8(font->font, line, &w, NULL);
  return w;
}

int graphics_Font_getWrap(graphics_Font const* font, char const* line, int width, char ** wrapped) {
  int linecount = 0;
  int len = strlen(line)+1;
  char *wbuf = malloc(len);
  int curwidth = 0;
  int curwordoffset = 0;

  for(int i = 0; i < len; ++i) {
    wbuf[i] = line[i];


  }

  strcpy(wbuf, line);


  printf("Wrapped:\n'%s'\n", wbuf);

}
