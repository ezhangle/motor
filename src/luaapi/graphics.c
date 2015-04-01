#include <lauxlib.h>
#include "graphics.h"
#include "tools.h"
#include "../graphics/graphics.h"
#include "../graphics/matrixstack.h"
#include "../graphics/shader.h"
#include "image.h"

#include "graphics_batch.h"
#include "graphics_canvas.h"
#include "graphics_image.h"
#include "graphics_quad.h"
#include "graphics_font.h"
#include "graphics_shader.h"
#include "graphics_window.h"
#include "graphics_geometry.h"

static int l_graphics_setBackgroundColor(lua_State* state) {
  int red   = lua_tointeger(state, 1);
  int green = lua_tointeger(state, 2);
  int blue  = lua_tointeger(state, 3);
  int alpha = luaL_optinteger(state, 4, 255);

  float scale = 1.0f / 255.0f;

  graphics_setBackgroundColor(red * scale, green * scale, blue * scale, alpha * scale);
  return 0;
}

static int l_graphics_setColor(lua_State* state) {
  int red   = lua_tointeger(state, 1);
  int green = lua_tointeger(state, 2);
  int blue  = lua_tointeger(state, 3);
  //int alpha = lua_tointeger(state, 4);
  int alpha = luaL_optinteger(state, 4, 255);

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
  //printf(")) Push\n");
  if(matrixstack_push()) {
    lua_pushstring(state, "Matrix stack overflow");
    return lua_error(state);
  }
  return 0;
}

static int l_graphics_pop(lua_State* state) {
  //printf(")) Pop\n");
  if(matrixstack_pop()) {
    lua_pushstring(state, "Matrix stack underrun");
    return lua_error(state);
  }
  return 0;
}

static int l_graphics_translate(lua_State* state) {
  float x = l_tools_tonumber_or_err(state, 1);
  float y = l_tools_tonumber_or_err(state, 2);

  //printf(")) Translate %f, %f\n", x, y);

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


static int l_graphics_getWidth(lua_State* state) {
  lua_pushnumber(state, graphics_getWidth());
  return 1;
}

static int l_graphics_getHeight(lua_State* state) {
  lua_pushnumber(state, graphics_getHeight());
  return 1;
}

static int l_graphics_reset(lua_State* state) {
  graphics_reset();
  return 0;
}


static luaL_Reg const regFuncs[] = {
  {"setBackgroundColor", l_graphics_setBackgroundColor},
  {"setColor",           l_graphics_setColor},
  {"clear",              l_graphics_clear},
  {"draw",               l_graphics_draw},
  {"push",               l_graphics_push},
  {"pop",                l_graphics_pop},
  {"origin",             l_graphics_origin},
  {"rotate",             l_graphics_rotate},
  {"scale",              l_graphics_scale},
  {"shear",              l_graphics_shear},
  {"translate",          l_graphics_translate},
  {"setColorMask",       l_graphics_setColorMask},
  {"getColorMask",       l_graphics_getColorMask},
  {"setBlendMode",       l_graphics_setBlendMode},
  {"getBlendMode",       l_graphics_getBlendMode},
  {"setScissor",         l_graphics_setScissor},
  {"getScissor",         l_graphics_getScissor},
  {"newShader",          l_graphics_newShader},
  {"getWidth",           l_graphics_getWidth},
  {"getHeight",          l_graphics_getHeight},
  {"reset",              l_graphics_reset},
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
  l_graphics_window_register(state);
  l_graphics_geometry_register(state);
  
  return 0;
}
