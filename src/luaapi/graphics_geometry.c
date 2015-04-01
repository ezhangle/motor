#include "graphics_geometry.h"
#include "../graphics/geometry.h"
#include "tools.h"

static const l_tools_Enum l_graphics_DrawMode[] = {
  {"fill", graphics_DrawMode_fill},
  {"line", graphics_DrawMode_line},
  {NULL, 0}
};

int l_geometry_circle(lua_State* state) {
  graphics_DrawMode mode = l_tools_toenum_or_err(state, 1, l_graphics_DrawMode);
  float x = l_tools_tonumber_or_err(state, 2);
  float y = l_tools_tonumber_or_err(state, 3);
  float r = l_tools_tonumber_or_err(state, 4);
  int segments;
  if(lua_isnoneornil(state, 5)) {
    segments = r > 10 ? (int)r : 10;
  } else  {
    segments = l_tools_tonumber_or_err(state, 5);
  }

  switch(mode) {
  case graphics_DrawMode_fill:
    graphics_geometry_fill_circle(x,y,r,segments);
    break;

  case graphics_DrawMode_line:
    // TODO
    break;
  }

  return 0;
}

static int l_graphics_rectangle(lua_State* state) {
  graphics_DrawMode mode = l_tools_toenum_or_err(state, 1, l_graphics_DrawMode);
  float x = l_tools_tonumber_or_err(state, 2);
  float y = l_tools_tonumber_or_err(state, 3);
  float w = l_tools_tonumber_or_err(state, 4);
  float h = l_tools_tonumber_or_err(state, 5);

  switch(mode) {
  case graphics_DrawMode_fill:
    graphics_geometry_fill_rectangle(x, y, w, h);
    break;

  case graphics_DrawMode_line:
    // TODO
    break;
  }

  return 0;
}


static luaL_Reg const geometryFreeFuncs[] = {
  {"circle",     l_geometry_circle},
  {"rectangle",  l_graphics_rectangle},
  {NULL, NULL}
};

void l_graphics_geometry_register(lua_State* state) {
  l_tools_register_funcs_in_module(state, "graphics", geometryFreeFuncs);
}

