#include <stdlib.h>
#include "graphics_geometry.h"
#include "../graphics/geometry.h"
#include "tools.h"

static struct {
  float * vertices;
  int vertSize;
} moduleData;

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
    graphics_geometry_draw_circle(x,y,r,segments);
    break;
  }

  return 0;
}

static int l_geometry_rectangle(lua_State* state) {
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
    graphics_geometry_draw_rectangle(x, y, w, h);
    break;
  }

  return 0;
}

static int l_geometry_line(lua_State* state) {
  bool table = lua_istable(state, 1);

  int count;
  if(table) {
    count = lua_objlen(state, 1);
  } else {
    count = lua_gettop(state);
  }

  if(count % 2) {
    lua_pushstring(state, "Need even number of values");
    return lua_error(state);
  }

  if(count < 4) {
    lua_pushstring(state, "Need at least two points for drawing lines");
    return lua_error(state);
  }

  if(count > moduleData.vertSize) {
    moduleData.vertices = realloc(moduleData.vertices, count * sizeof(float));
    moduleData.vertSize = count;
  }

  if(table) {
    for(int i = 0; i < count; ++i) {
      lua_rawgeti(state, -1, i);
      moduleData.vertices[i] = l_tools_tonumber_or_err(state, -1);
      lua_pop(state, 1);
    }
  } else {
    for(int i = 0; i < count; ++i) {
      moduleData.vertices[i] = l_tools_tonumber_or_err(state, 1 + i);
    }
  }

  graphics_geometry_draw_lines(count/2, moduleData.vertices);

  return 0;
}


static int l_geometry_setLineWidth(lua_State* state) {
  float width = l_tools_tonumber_or_err(state, 1);
  graphics_geometry_set_line_width(width);
  return 0;
}

static int l_geometry_getLineWidth(lua_State* state) {
  lua_pushnumber(state, graphics_geometry_get_line_width());
  return 1;
}


static luaL_Reg const geometryFreeFuncs[] = {
  {"circle",       l_geometry_circle},
  {"rectangle",    l_geometry_rectangle},
  {"line",         l_geometry_line},
  {"setLineWidth", l_geometry_setLineWidth},
  {"getLineWidth", l_geometry_getLineWidth},
  {NULL, NULL}
};

void l_graphics_geometry_register(lua_State* state) {
  l_tools_register_funcs_in_module(state, "graphics", geometryFreeFuncs);
}

