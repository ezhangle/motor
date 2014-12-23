#include <lauxlib.h>
#include "boot.h"

static char const bootScript[] =
  "motor.update = function(dt) end\n"
  "motor.draw = function() end\n"
  "motor.load = function() end\n"
  "local conf = {\n"
  "  window = {\n"
  "    width = 400,\n"
  "    height = 400\n"
  "  }\n"
  "}\n"
  "local confFunc = loadfile(\"conf.lua\")\n"
  "if confFunc then\n"
  "  confFunc()\n"
  "  motor.conf(conf)\n"
  "end\n"
  "return conf\n"
;

int l_boot(lua_State* state, motor_Config *config) {
  if(luaL_dostring(state, bootScript)) {
    return 1;
  }

  lua_pushstring(state, "window");
  lua_rawget(state, -2);

  lua_pushstring(state, "width");
  lua_rawget(state, -2);

  config->window.width = lua_tointeger(state, -1);
  lua_pop(state, 1);
  
  lua_pushstring(state, "height");
  lua_rawget(state, -2);
  config->window.height = lua_tointeger(state, -1);

  lua_pop(state, 3);

  return 0;

}
