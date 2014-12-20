#include <lauxlib.h>
#include "lua_boot.h"

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
  "print(conf.window.width, conf.window.height)\n"
;

int motor_lua_boot(lua_State* state) {
  return luaL_dostring(state, bootScript);
}
