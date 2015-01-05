#define _POSIX_SOURCE
#include <string.h>
#include <tgmath.h>
#include "font.h"

extern void graphics_font_init();
extern int graphics_Font_new(graphics_Font *dst, char const* filename, int ptsize);
extern void graphics_Font_free(graphics_Font *obj);
extern int graphics_Font_getHeight(graphics_Font const* font); 
extern int graphics_Font_getAscent(graphics_Font const* font);
extern int graphics_Font_getDescent(graphics_Font const* font);
extern int graphics_Font_getBaseline(graphics_Font const* font);
extern int graphics_Font_getWidth(graphics_Font const* font, char const* line);

int graphics_Font_getWrap(graphics_Font const* font, char const* text, int width, char ** wrapped) {
  int linecount = 0;
  int len = strlen(text);
  char *wbuf = malloc(len+1);
  char *out = NULL;

  if(wrapped) {
    *wrapped = malloc(len+1);
    out = *wrapped;
  }

  memcpy(wbuf, text, len+1);

  char *line_save;
  int spaceWidth;
  TTF_GlyphMetrics(font->font, ' ', NULL, NULL, NULL, NULL, &spaceWidth);
  char *line = strtok_r(wbuf, "\n", &line_save);
  char *curline;
  while(line) {
    if(linecount != 0) {
      *out = '\n';
      ++out;
    }

    ++linecount;
    char *word_save;
    char *word = strtok_r(line, " ", &word_save);
    int curwidth = 0;
    while(word) {
      int wlen = strlen(word);
      int ww = graphics_Font_getWidth(font, word);
      if(curwidth == 0) {
        curwidth = ww;
      } else {
        if(curwidth + ww + spaceWidth > width) {
          *out = '\n';
          curwidth = ww;
        } else {
          curwidth += ww + spaceWidth;
          *out = ' ';
        }
        ++out;
      }
      strcpy(out, word);
      out += wlen;

      word = strtok_r(NULL, " ", &word_save);
    }

    line = strtok_r(NULL, "\n", &line_save);
  }

  strcpy(wbuf, line);
}
