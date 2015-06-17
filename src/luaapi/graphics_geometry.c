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

static const l_tools_Enum l_graphics_LineJoin[] = {
  {"none",  graphics_LineJoin_none},
  {"bevel", graphics_LineJoin_bevel},
  {"miter", graphics_LineJoin_miter},
  {NULL, 0}
};

int l_geometry_circle(lua_State* state) {
  graphics_DrawMode mode = l_tools_toEnumOrError(state, 1, l_graphics_DrawMode);
  float x = l_tools_toNumberOrError(state, 2);
  float y = l_tools_toNumberOrError(state, 3);
  float r = l_tools_toNumberOrError(state, 4);
  int segments;
  if(lua_isnoneornil(state, 5)) {
    segments = r > 10 ? (int)r : 10;
  } else  {
    segments = l_tools_toNumberOrError(state, 5);
  }

  switch(mode) {
  case graphics_DrawMode_fill:
    graphics_geometry_fillCircle(x,y,r,segments);
    break;

  case graphics_DrawMode_line:
    graphics_geometry_drawCircle(x,y,r,segments);
    break;
  }

  return 0;
}

static int l_geometry_rectangle(lua_State* state) {
  graphics_DrawMode mode = l_tools_toEnumOrError(state, 1, l_graphics_DrawMode);
  float x = l_tools_toNumberOrError(state, 2);
  float y = l_tools_toNumberOrError(state, 3);
  float w = l_tools_toNumberOrError(state, 4);
  float h = l_tools_toNumberOrError(state, 5);

  switch(mode) {
  case graphics_DrawMode_fill:
    graphics_geometry_fillRectangle(x, y, w, h);
    break;

  case graphics_DrawMode_line:
    graphics_geometry_drawRectangle(x, y, w, h);
    break;
  }

  return 0;
}


static int l_geometry_read_vertices(lua_State* state, int offset) {
  bool table = lua_istable(state, 1 + offset);

  int count;
  if(table) {
    count = lua_objlen(state, 1 + offset);
  } else {
    count = lua_gettop(state) - offset;
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
      moduleData.vertices[i] = l_tools_toNumberOrError(state, -1);
      lua_pop(state, 1);
    }
  } else {
    for(int i = 0; i < count; ++i) {
      moduleData.vertices[i] = l_tools_toNumberOrError(state, 1 + i + offset);
    }
  }

  return count / 2;
}


static int l_geometry_line(lua_State* state) {
  int count = l_geometry_read_vertices(state, 0);
  graphics_geometry_drawLines(count, moduleData.vertices);
  return 0;
}


static int l_geometry_polygon(lua_State* state) {
  int count = l_geometry_read_vertices(state, 1);
  graphics_DrawMode mode = l_tools_toEnumOrError(state, 1, l_graphics_DrawMode);
  switch(mode) {
  case graphics_DrawMode_fill:
    graphics_geometry_fillPolygon(count, moduleData.vertices);
    break;

  case graphics_DrawMode_line:
    graphics_geometry_drawPolygon(count, moduleData.vertices);
    break;
  }
  return 0;
}


static int l_geometry_setLineWidth(lua_State* state) {
  float width = l_tools_toNumberOrError(state, 1);
  graphics_geometry_setLineWidth(width);
  return 0;
}


static int l_geometry_getLineWidth(lua_State* state) {
  lua_pushnumber(state, graphics_geometry_getLineWidth());
  return 1;
}


static int l_geometry_setLineJoin(lua_State* state) {
  graphics_LineJoin join = l_tools_toEnumOrError(state, 1, l_graphics_LineJoin);
  if(join == graphics_LineJoin_bevel) {
    lua_pushstring(state, "'bevel' line join mode not supported yet");
    return lua_error(state);
  }

  graphics_geometry_setLineJoin(join);
  return 0;
}


static int l_geometry_getLineJoin(lua_State* state) {
  l_tools_pushEnum(state, graphics_geometry_getLineJoin(), l_graphics_LineJoin);
  return 1;
}


static luaL_Reg const geometryFreeFuncs[] = {
  {"circle",       l_geometry_circle},
  {"rectangle",    l_geometry_rectangle},
  {"line",         l_geometry_line},
  {"polygon",      l_geometry_polygon},
  {"setLineWidth", l_geometry_setLineWidth},
  {"getLineWidth", l_geometry_getLineWidth},
  {"setLineJoin",  l_geometry_setLineJoin},
  {"getLineJoin",  l_geometry_getLineJoin},
  {NULL, NULL}
};


void l_graphics_geometry_register(lua_State* state) {
  l_tools_registerFuncsInModule(state, "graphics", geometryFreeFuncs);
}

