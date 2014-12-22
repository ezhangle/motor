#pragma once

#include <lualib.h>
#include "motor_config.h"

int motor_lua_boot(lua_State* state, motor_Config *config);
