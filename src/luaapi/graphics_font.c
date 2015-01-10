#include <lauxlib.h>
#include "tools.h"
#include "graphics_font.h"
#include "../graphics/font.h"

static struct {
  int fontMT;
  graphics_Font* currentFont;
  int currentFontRef;
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
  int x = l_tools_tonumber_or_err(state, 2);
  int y = l_tools_tonumber_or_err(state, 3);

  graphics_Font_render(moduleData.currentFont, text);
  return 0;
}

int l_graphics_newFont(lua_State* state) {
  // TODO: alternative signatures for newFont
  char const * filename = l_tools_tostring_or_err(state, 1);
  int ptsize = l_tools_tonumber_or_err(state, 2);

  graphics_Font* font = lua_newuserdata(state, sizeof(graphics_Font));
  if(graphics_Font_new(font, filename, ptsize)) {
    lua_pushstring(state, "Could not open font");
    lua_error(state);
  }

  lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.fontMT);
  lua_setmetatable(state, -2);
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

  lua_pushinteger(state, graphics_Font_getHeight(font));
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

  lua_pushinteger(state, graphics_Font_getWidth(font, line));
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

}
