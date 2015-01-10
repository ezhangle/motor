#include <lauxlib.h>
#include "graphics.h"
#include "tools.h"
#include "../graphics/graphics.h"
#include "../graphics/matrixstack.h"
#include "../graphics/batch.h"
#include "../graphics/canvas.h"
#include "../graphics/shader.h"
#include "image.h"

#include "graphics_batch.h"
#include "graphics_canvas.h"
#include "graphics_image.h"
#include "graphics_quad.h"
#include "graphics_font.h"
#include "graphics_shader.h"

static struct {
  graphics_Font* currentFont;
  int currentFontRef;
  graphics_Canvas* currentCanvas;
  int currentCanvasRef;
} moduleData;

static int l_graphics_setBackgroundColor(lua_State* state) {
  int red   = lua_tointeger(state, 1);
  int green = lua_tointeger(state, 2);
  int blue  = lua_tointeger(state, 3);
  int alpha = lua_tointeger(state, 4);

  float scale = 1.0f / 255.0f;

  graphics_setBackgroundColor(red * scale, green * scale, blue * scale, alpha * scale);
  return 0;
}

static int l_graphics_setColor(lua_State* state) {
  int red   = lua_tointeger(state, 1);
  int green = lua_tointeger(state, 2);
  int blue  = lua_tointeger(state, 3);
  int alpha = lua_tointeger(state, 4);

  float scale = 1.0f / 255.0f;

  graphics_setColor(red * scale, green * scale, blue * scale, alpha * scale);
  return 0;
}

static int l_graphics_clear(lua_State* state) {
  graphics_clear();
  return 0;
}

static const graphics_Quad defaultQuad = {
  .x = 0.0,
  .y = 0.0,
  .w = 1.0,
  .h = 1.0
};
static int l_graphics_draw(lua_State* state) {
  l_graphics_Image const * image = NULL;
  l_graphics_Batch const * batch = NULL;
  graphics_Canvas const * canvas = NULL;
  graphics_Quad const * quad = &defaultQuad;
  int baseidx = 2;

  if(l_graphics_isImage(state, 1)) {
    if(l_graphics_isQuad(state, 2)) {
      quad = l_graphics_toQuad(state, 2);
      baseidx = 3;
    }
    image = l_graphics_toImage(state, 1);
  } else if(l_graphics_isCanvas(state, 1)) {
    if(l_graphics_isQuad(state, 2)) {
      quad = l_graphics_toQuad(state, 2);
      baseidx = 3;
    }
    canvas = l_graphics_toCanvas(state, 1);

  } else if(l_graphics_isBatch(state, 1)) {
    batch = l_graphics_toBatch(state, 1);
  } else {
    lua_pushstring(state, "expected image or spritebatch");
    lua_error(state);
  }
  
  float x  = luaL_optnumber(state, baseidx,     0.0f);
  float y  = luaL_optnumber(state, baseidx + 1, 0.0f);
  float r  = luaL_optnumber(state, baseidx + 2, 0.0f);
  float sx = luaL_optnumber(state, baseidx + 3, 1.0f);
  float sy = luaL_optnumber(state, baseidx + 4, sx);
  float ox = luaL_optnumber(state, baseidx + 5, 0.0f);
  float oy = luaL_optnumber(state, baseidx + 6, 0.0f);
  float kx = luaL_optnumber(state, baseidx + 7, 0.0f);
  float ky = luaL_optnumber(state, baseidx + 8, 0.0f);

  if(image) {
    graphics_Image_draw(&image->image, quad, x, y, r, sx, sy, ox, oy, kx, ky);
  } else if(canvas) {
    graphics_Canvas_draw(canvas, quad, x, y, r, sx, sy, ox, oy, kx, ky);
  } else if(batch) {
    graphics_Batch_draw(&batch->batch, x, y, r, sx, sy, ox, oy, kx, ky);
  }
  return 0;
}

static int l_graphics_push(lua_State* state) {
  if(matrixstack_push()) {
    lua_pushstring(state, "Matrix stack overflow");
    return lua_error(state);
  }
  return 0;
}

static int l_graphics_pop(lua_State* state) {
  if(matrixstack_pop()) {
    lua_pushstring(state, "Matrix stack underrun");
    return lua_error(state);
  }
  return 0;
}

static int l_graphics_translate(lua_State* state) {
  float x = l_tools_tonumber_or_err(state, 1);
  float y = l_tools_tonumber_or_err(state, 2);

  matrixstack_translate(x, y);
  return 0;
}

static int l_graphics_scale(lua_State* state) {
  float x = l_tools_tonumber_or_err(state, 1);
  float y = l_tools_tonumber_or_err(state, 2);

  matrixstack_scale(x, y);
  return 0;
}

static int l_graphics_origin(lua_State* state) {
  matrixstack_origin();
  return 0;
}

static int l_graphics_shear(lua_State* state) {
  lua_pushstring(state, "not implemented");
  lua_error(state);
  return 0;
}

static int l_graphics_rotate(lua_State* state) {
  float a = l_tools_tonumber_or_err(state, 1);

  matrixstack_rotate(a);
  return 0;
}



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

static int l_graphics_setCanvas(lua_State* state) {
  graphics_Canvas *canvas = NULL;

  if(l_graphics_isCanvas(state, 1)) {
    canvas = l_graphics_toCanvas(state, 1);
  } else if(!lua_isnoneornil(state, 1)) {
    lua_pushstring(state, "expected none or canvas");
    return lua_error(state);
  }

  if(moduleData.currentCanvas) {
    moduleData.currentCanvas = NULL;
    luaL_unref(state, LUA_REGISTRYINDEX, moduleData.currentFontRef);
  }

  // TODO support multiple canvases
  lua_settop(state, 1);

  graphics_setCanvas(canvas);
  if(canvas) {
    moduleData.currentCanvas = canvas;
    moduleData.currentFontRef = luaL_ref(state, LUA_REGISTRYINDEX);
  }

  return 0;
}

static int l_graphics_setColorMask(lua_State* state) {
  if(lua_isnone(state, 1)) {
    graphics_setColorMask(true, true, true, true);
    return 0;
  } else {
    for(int i = 2; i < 5; ++i) {
      if(lua_isnone(state, i)) {
        lua_pushstring(state, "illegal paramters");
        return lua_error(state);
      }
    }
  }

  bool r = lua_toboolean(state, 1);
  bool g = lua_toboolean(state, 2);
  bool b = lua_toboolean(state, 3);
  bool a = lua_toboolean(state, 4);

  graphics_setColorMask(r,g,b,a);

  return 0;
}

static int l_graphics_getColorMask(lua_State* state) {
  bool r,g,b,a;
  graphics_getColorMask(&r, &g, &b, &a);
  lua_pushboolean(state, r);
  lua_pushboolean(state, g);
  lua_pushboolean(state, b);
  lua_pushboolean(state, a);

  return 4;
}

static const l_tools_Enum l_graphics_BlendMode[] = {
  {"additive",       graphics_BlendMode_additive},
  {"alpha",          graphics_BlendMode_alpha},
  {"subtractive",    graphics_BlendMode_subtractive},
  {"multiplicative", graphics_BlendMode_multiplicative},
  {"premultiplied",  graphics_BlendMode_premultiplied},
  {"replace",        graphics_BlendMode_replace},
  {"screen",         graphics_BlendMode_screen},
  {NULL, 0}
};

static int l_graphics_setBlendMode(lua_State* state) {
  graphics_BlendMode mode = l_tools_toenum_or_err(state, 1, l_graphics_BlendMode);
  graphics_setBlendMode(mode);
  return 0;
}

static int l_graphics_getBlendMode(lua_State* state) {
  l_tools_pushenum(state, graphics_getBlendMode(), l_graphics_BlendMode);
  return 1;
}

static int l_graphics_setScissor(lua_State* state) {
  if(lua_isnone(state, 1)) {
    graphics_clearScissor();
    return 0;
  } else {
    for(int i = 2; i < 5; ++i) {
      if(lua_isnone(state, i)) {
        lua_pushstring(state, "illegal paramters");
        return lua_error(state);
      }
    }
  }

  int x = l_tools_tonumber_or_err(state, 1);
  int y = l_tools_tonumber_or_err(state, 2);
  int w = l_tools_tonumber_or_err(state, 3);
  int h = l_tools_tonumber_or_err(state, 4);

  graphics_setScissor(x,y,w,h);

  return 0;
}

static int l_graphics_getScissor(lua_State* state) {
  int x,y,w,h;
  bool scissor = graphics_getScissor(&x, &y, &w, &h);
  if(!scissor) {
    return 0;
  }

  lua_pushinteger(state, x);
  lua_pushinteger(state, y);
  lua_pushinteger(state, w);
  lua_pushinteger(state, h);

  return 4;
}


static luaL_Reg const regFuncs[] = {
  {"setBackgroundColor", l_graphics_setBackgroundColor},
  {"setColor",           l_graphics_setColor},
  {"clear",              l_graphics_clear},
  {"newImage",           l_graphics_newImage},
  {"newQuad",            l_graphics_newQuad},
  {"draw",               l_graphics_draw},
  {"push",               l_graphics_push},
  {"pop",                l_graphics_pop},
  {"origin",             l_graphics_origin},
  {"rotate",             l_graphics_rotate},
  {"scale",              l_graphics_scale},
  {"shear",              l_graphics_shear},
  {"translate",          l_graphics_translate},
  {"newFont",            l_graphics_newFont},
  {"setFont",            l_graphics_setFont},
  {"printf",             l_graphics_printf},
  {"print",              l_graphics_print},
  {"newSpriteBatch",     l_graphics_newSpriteBatch},
  {"newCanvas",          l_graphics_newCanvas},
  {"setCanvas",          l_graphics_setCanvas},
  {"setColorMask",       l_graphics_setColorMask},
  {"getColorMask",       l_graphics_getColorMask},
  {"setBlendMode",       l_graphics_setBlendMode},
  {"getBlendMode",       l_graphics_getBlendMode},
  {"setScissor",         l_graphics_setScissor},
  {"getScissor",         l_graphics_getScissor},
  {"newShader",          l_graphics_newShader},
  {NULL, NULL}
};




int l_graphics_register(lua_State* state) {
  l_tools_register_module(state, "graphics", regFuncs);
  
  l_graphics_image_register(state);
  l_graphics_quad_register(state);
  l_graphics_font_register(state);
  l_graphics_batch_register(state);
  l_graphics_canvas_register(state);
  l_graphics_shader_register(state);
  
  moduleData.currentFont = NULL;

  return 0;
}
