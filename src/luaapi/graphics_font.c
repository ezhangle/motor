#include <lauxlib.h>
#include "tools.h"
#include "graphics_font.h"
#include "../graphics/font.h"

#include "../graphics/matrixstack.h"
#include "../graphics/shader.h"
#include "../graphics/graphics.h"

static struct {
  int fontMT;
  graphics_Font* currentFont;
  int currentFontRef;
  int loadedFontsRef;
} moduleData;

static int l_graphics_setFont(lua_State* state) {
  if(!l_graphics_isFont(state, 1)) {
    lua_pushstring(state, "expected font");
    lua_error(state);
  }

  lua_settop(state, 1);

  graphics_Font* font = l_graphics_toFont(state, 1);

  // Release current font in Lua, so it can be GCed if needed
  if(moduleData.currentFont) {
    luaL_unref(state, LUA_REGISTRYINDEX, moduleData.currentFontRef);
  }

  moduleData.currentFontRef = luaL_ref(state, LUA_REGISTRYINDEX);
  moduleData.currentFont = font;

  return 0;
}

static int l_graphics_printf(lua_State* state) {
  char const* text = l_tools_tostring_or_err(state, 1);
  return 0;
}

static int l_graphics_print(lua_State* state) {
  char const* text = l_tools_tostring_or_err(state, 1);
  printf("printing '%s'\n", text);
  int x = l_tools_tonumber_or_err(state, 2);
  int y = l_tools_tonumber_or_err(state, 3);

  graphics_Font_render(moduleData.currentFont, text, x, y);
  return 0;
}

int l_graphics_newFont(lua_State* state) {
  // TODO: alternative signatures for newFont
  char const * filename = l_tools_tostring_or_err(state, 1);
  int ptsize = l_tools_tonumber_or_err(state, 2);
  
  // Create string font:size
  // Stack: ... fontname
  lua_pushstring(state, ":");
  lua_insert(state, -2);
  lua_concat(state, 3);

  // Load font table to -2
  // Stack: ... fonts fontname
  lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.loadedFontsRef);
  lua_insert(state, -2);

  // Save fontname for later
  // Stack: ... fonts fontname fontname
  lua_pushvalue(state, -1);

  // Load font
  // Stack: ... fonts fontname maybefont
  lua_gettable(state, -3);

  if(lua_isnoneornil(state, -1)) {
    // Stack: ... fonts fontname
    lua_pop(state, 1);

    // Stack: ... fonts fontname raw-font
    graphics_Font* font = lua_newuserdata(state, sizeof(graphics_Font));
    if(graphics_Font_new(font, filename, ptsize)) {
      lua_pushstring(state, "Could not open font");
      lua_error(state);
    }

    // Stack: ... fonts fontname raw-font metatable
    lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.fontMT);

    // Stack: ... fonts fontname constructed-font
    lua_setmetatable(state, -2);

    // Stack: ... fonts fontname constructed-font constructed-font
    lua_pushvalue(state, -1);

    // Stack: ... fonts constructed-font fontname constructed-font
    lua_insert(state, -3);

    // Stack: ... fonts constructed-font
    lua_settable(state, -4);
  }
  return 1;
}

static int l_graphics_gcFont(lua_State* state) {
  graphics_Font* font = l_graphics_toFont(state, 1);
  graphics_Font_free(font);
  return 0;
}

static int l_graphics_Font_getHeight(lua_State* state) {
  if(!l_graphics_isFont(state, 1)) {
    lua_pushstring(state, "expected font");
    lua_error(state);
  }

  graphics_Font* font = l_graphics_toFont(state, 1);
  
  int height = graphics_Font_getHeight(font);
  lua_pushinteger(state, height);
  return 1;
}

static int l_graphics_Font_getDescent(lua_State* state) {
  if(!l_graphics_isFont(state, 1)) {
    lua_pushstring(state, "expected font");
    lua_error(state);
  }

  graphics_Font* font = l_graphics_toFont(state, 1);

  lua_pushinteger(state, graphics_Font_getDescent(font));
  return 1;
}

static int l_graphics_Font_getAscent(lua_State* state) {
  if(!l_graphics_isFont(state, 1)) {
    lua_pushstring(state, "expected font");
    lua_error(state);
  }

  graphics_Font* font = l_graphics_toFont(state, 1);

  lua_pushinteger(state, graphics_Font_getAscent(font));
  return 1;
}

static int l_graphics_Font_getBaseline(lua_State* state) {
  if(!l_graphics_isFont(state, 1)) {
    lua_pushstring(state, "expected font");
    lua_error(state);
  }

  graphics_Font* font = l_graphics_toFont(state, 1);

  lua_pushinteger(state, graphics_Font_getBaseline(font));
  return 1;
}

static int l_graphics_Font_getWidth(lua_State* state) {
  if(!l_graphics_isFont(state, 1)) {
    lua_pushstring(state, "expected font");
    lua_error(state);
  }

  graphics_Font* font = l_graphics_toFont(state, 1);

  char const* line = l_tools_tostring_or_err(state, 2);
  int width = graphics_Font_getWidth(font, line);

  lua_pushinteger(state, width);
  return 1;
}

static int l_graphics_Font_getWrap(lua_State* state) {
  if(!l_graphics_isFont(state, 1)) {
    lua_pushstring(state, "expected font");
    lua_error(state);
  }

  graphics_Font* font = l_graphics_toFont(state, 1);

  char const* line = l_tools_tostring_or_err(state, 2);
  int width = l_tools_tonumber_or_err(state, 3);

  lua_pushinteger(state, graphics_Font_getWrap(font, line, width, NULL));
  return 1;
}


static luaL_Reg const fontMetatableFuncs[] = {
  {"__gc",               l_graphics_gcFont},
  {"getHeight",          l_graphics_Font_getHeight},
  {"getAscent",          l_graphics_Font_getAscent},
  {"getDescent",         l_graphics_Font_getDescent},
  {"getBaseline",        l_graphics_Font_getBaseline},
  {"getWidth",           l_graphics_Font_getWidth},
  {"getWrap",            l_graphics_Font_getWrap},
  {NULL, NULL}
};

static luaL_Reg const fontFreeFuncs[] = {
  {"newFont",            l_graphics_newFont},
  {"setFont",            l_graphics_setFont},
  {"printf",             l_graphics_printf},
  {"print",              l_graphics_print},
  {NULL, NULL}
};

l_check_type_fn(l_graphics_isFont, moduleData.fontMT)
l_to_type_fn(l_graphics_toFont, graphics_Font)

void l_graphics_font_register(lua_State* state) {
  l_tools_register_funcs_in_module(state, "graphics", fontFreeFuncs);
  moduleData.fontMT   = l_tools_make_type_mt(state, fontMetatableFuncs);
  moduleData.currentFont = NULL;

  lua_newtable(state);
  lua_newtable(state);
  lua_pushstring(state, "__mode");
  lua_pushstring(state, "v");
  lua_rawset(state, -3);
  lua_setmetatable(state, -2);
  moduleData.loadedFontsRef = luaL_ref(state, LUA_REGISTRYINDEX);
}

void l_graphics_font_debug() {
  matrixstack_origin();
  graphics_setColor(1.0f, 1.0f, 1.0f, 0.2f);
  graphics_setDefaultShader();
  graphics_Image img = {
    NULL, moduleData.currentFont->glyphs.textures[0],
    256, 256
  };
//  printf("Debugging... %d\n", img.texID);
  graphics_Quad quad = {
    0,0,1,1
  };
  graphics_Image_draw(&img, &quad, 0, 0, 0, 1, 1, 0, 0, 0, 0);
}
