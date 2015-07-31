#include "../mouse.h"
#include "tools.h"


static int l_mouse_isVisible(lua_State *state) {
  lua_pushboolean(state, mouse_isVisible());
  return 1;
}


static int l_mouse_isDown(lua_State *state) {
  const char *name = luaL_checkstring(state, 1);
  int res = mouse_isDown(name);
  if(res < 0) {
    luaL_error(state, "bad button name '%s'", name);
  }
  lua_pushboolean(state, res);
  return 1;
}


static int l_mouse_getPosition(lua_State *state) {
  int x, y;
  mouse_getPosition(&x, &y);
  lua_pushinteger(state, x);
  lua_pushinteger(state, y);
  return 2;
}


static int l_mouse_getX(lua_State *state) {
  lua_pushinteger(state, mouse_getX());
  return 1;
} 


static int l_mouse_getY(lua_State *state) {
  lua_pushinteger(state, mouse_getY());
  return 1;
} 


static int l_mouse_setPosition(lua_State *state) {
  lua_pushstring(state, "love.mouse.setPosition not supported in Motor2D. There is no HTML5 API to do that. Sorry.");
  return lua_error(state);
} 


static int l_mouse_setVisible(lua_State *state) {
  mouse_setVisible(lua_toboolean(state, 1));
  return 0;
}


static int l_mouse_setX(lua_State *state) {
  lua_pushstring(state, "love.mouse.setX not supported in Motor2D. There is no HTML5 API to do that. Sorry.");
  return lua_error(state);
} 


static int l_mouse_setY(lua_State *state) {
  lua_pushstring(state, "love.mouse.setY not supported in Motor2D. There is no HTML5 API to do that. Sorry.");
  return lua_error(state);
} 


static void l_mouse_pressed(void *ud, int x, int y, int button) {
  lua_State * state = (lua_State*)ud;
  lua_getglobal(state, "love");
  lua_pushstring(state, "mousepressed");
  lua_rawget(state, -2);
  lua_pushnumber(state, x);
  lua_pushnumber(state, y);
  lua_pushstring(state, mouse_getButtonName(button));
  lua_call(state, 3, 0);
}


static void l_mouse_released(void *ud, int x, int y, int button) {
  lua_State * state = (lua_State*)ud;
  lua_getglobal(state, "love");
  lua_pushstring(state, "mousereleased");
  lua_rawget(state, -2);
  lua_pushnumber(state, x);
  lua_pushnumber(state, y);
  lua_pushstring(state, mouse_getButtonName(button));
  lua_call(state, 3, 0);
}


static void l_mouse_moved(void *ud, int x, int y, int dx, int dy) {
  lua_State * state = (lua_State*)ud;
  lua_getglobal(state, "love");
  lua_pushstring(state, "mousemoved");
  lua_rawget(state, -2);
  lua_pushnumber(state, x);
  lua_pushnumber(state, y);
  lua_pushnumber(state, dx);
  lua_pushnumber(state, dy);
  lua_call(state, 4, 0);
}


static luaL_Reg const regFuncs[] = {
  { "isDown",         l_mouse_isDown        },
  { "isVisible",      l_mouse_isVisible     },
  { "getPosition",    l_mouse_getPosition   },
  { "getX",           l_mouse_getX          },
  { "getY",           l_mouse_getY          },
  { "setPosition",    l_mouse_setPosition   },
  { "setVisible",     l_mouse_setVisible    },
  { "setX",           l_mouse_setX          },
  { "setY",           l_mouse_setY          },
  { NULL, NULL }
};


void l_mouse_register(lua_State* state) {
  l_tools_registerModule(state, "mouse", regFuncs);

  mouse_EventCallbacks callbacks = {
    .pressed  = l_mouse_pressed,
    .released = l_mouse_released,
    .moved    = l_mouse_moved,
    .userData = state
  };

  mouse_setEventCallbacks(&callbacks);
}
