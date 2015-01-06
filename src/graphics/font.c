#define _POSIX_SOURCE
#include <string.h>
#include <tgmath.h>
#include "font.h"
#include <stdlib.h>
#include "quad.h"
#include "graphics.h"
#include "../tools/utf8.h"

// TODO fixing the texture size may be a bad idea. Trying to find a 
//      better estimate of the required texture size (like love does)
//      could improve performance
static const int GlyphTextureWidth = 256;
static const int GlyphTextureHeight = 256;

static struct {
  FT_Library ft;
} moduleData;


void graphics_GlyphMap_newTexture(graphics_GlyphMap *map) {
  map->textures = realloc(map->textures, sizeof(GLuint) * (map->numTextures + 1));
  glGenTextures(1, &map->textures[map->numTextures]);
  glBindTexture(GL_TEXTURE_2D, map->textures[map->numTextures]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, GlyphTextureWidth, GlyphTextureHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  ++map->numTextures;
}

void graphics_GlyphMap_new(graphics_GlyphMap *map) {
  memset(map, 0, sizeof(graphics_GlyphMap));
  graphics_GlyphMap_newTexture(map);
}

void graphics_GlyphMap_free(graphics_GlyphMap* map) {
  glDeleteTextures(map->numTextures, map->textures);
  free(map->textures);
}

graphics_Glyph const* graphics_Font_findGlyph(graphics_Font *font, unsigned unicode) {

}

int graphics_Font_new(graphics_Font *dst, char const* filename, int ptsize) {
  int error = FT_New_Face(moduleData.ft, filename, 0, &dst->face);
  FT_Set_Pixel_Sizes(dst->face, 0, ptsize);

  graphics_GlyphMap_new(&dst->glyphs);

  printf("font_new: %d\n", error);


  return 0;
}

void graphics_Font_free(graphics_Font *obj) {
  graphics_GlyphMap_free(&obj->glyphs);
}

int graphics_Font_getWrap(graphics_Font const* font, char const* text, int width, char ** wrapped) {
  // TODO with some brains this can be made even faster by removing strtok_r and parsing the text
  //      directly, without separating it into lines and words first.
  //      The current version scans the text up to 3x, once to split it into lines, once to split
  //      each line into words and once to move the word to the appropriate output position.
  //      The task at hand can in principle be done reading the text only once.
  
  int linecount = 0;
  int len = strlen(text);
  char *wbuf = malloc(len+1);
  char *out = wbuf;

  memcpy(wbuf, text, len+1);

  char *line_save;
  int spaceWidth;
//  TTF_GlyphMetrics(font->font, ' ', NULL, NULL, NULL, NULL, &spaceWidth);
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

static const SDL_Color white = {255,255,255,255};
void graphics_Font_render(graphics_Font* font, char const* text) {
  graphics_Image img = {
    0,
    font->glyphs.textures[0],
    256, 256 };

  graphics_Quad q = {0,0,1,1};
  graphics_draw_Image(&img, &q, 0,0,0,1,1,0,0,0,0);

  char const* txt = text;
  printf("Rendering:");
  uint8_t cp;
  while(cp = utf8_scan(&text)) {
    printf(" %08x", cp);
  }
  printf("\n");

/*
  SDL_FreeSurface(font->imageData.surface);
  font->imageData.surface = TTF_RenderText_Blended(font->font, text, white);
  graphics_Image_refresh(&font->texture);
    
  graphics_Filter filter;
  graphics_Image_getFilter(&font->texture, &filter);
*/

  //return &font->texture;
}

void graphics_font_init() {
  int error = FT_Init_FreeType(&moduleData.ft);
  printf("FT init: %d\n", error);
}

int graphics_Font_getHeight(graphics_Font const* font) {
}

int graphics_Font_getAscent(graphics_Font const* font) {
}

int graphics_Font_getDescent(graphics_Font const* font) {
}

int graphics_Font_getBaseline(graphics_Font const* font) {
}

int graphics_Font_getWidth(graphics_Font const* font, char const* line) {
}

