#pragma once

#include <lualib.h>
#include "../motor_config.h"

int l_boot(lua_State* state, motor_Config *config);
