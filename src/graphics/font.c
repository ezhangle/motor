#define _POSIX_SOURCE
#include <string.h>
#include <tgmath.h>
#include "font.h"
#include <stdlib.h>

extern void graphics_font_init();
extern int graphics_Font_new(graphics_Font *dst, char const* filename, int ptsize);
extern void graphics_Font_free(graphics_Font *obj);
extern int graphics_Font_getHeight(graphics_Font const* font); 
extern int graphics_Font_getAscent(graphics_Font const* font);
extern int graphics_Font_getDescent(graphics_Font const* font);
extern int graphics_Font_getBaseline(graphics_Font const* font);
extern int graphics_Font_getWidth(graphics_Font const* font, char const* line);

int graphics_Font_getWrap(graphics_Font const* font, char const* text, int width, char ** wrapped) {
  // TODO with some brains this can be made even faster by removing strtok_r and parsing the text
  //      directly, without separating it into lines and words first.
  //      The current version scans the text up to 3x, once to split it into lines, once to split
  //      each line into words and once to move the word to the appropriate output position.
  //      The task at hand can in principle be done reading the text only once.
  
  int linecount = 0;
  int len = strlen(text);
  char *wbuf = malloc(len+1);
  char *out = NULL;

  memcpy(wbuf, text, len+1);

  char *line_save;
  int spaceWidth;
  TTF_GlyphMetrics(font->font, ' ', NULL, NULL, NULL, NULL, &spaceWidth);
  char *line = strtok_r(wbuf, "\n", &line_save);
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
          ++linecount;
          curwidth = ww;
        } else {
          curwidth += ww + spaceWidth;
          *out = ' ';
        }
        ++out;
      }
      if(out != word) {
        memmove(out, word, wlen + 1);
      }
      out += wlen;

      word = strtok_r(NULL, " ", &word_save);
    }

    line = strtok_r(NULL, "\n", &line_save);
  }

  if(wrapped) {
    *wrapped = wbuf;
  } else {
    free(wbuf);
  }

  return linecount;
}
