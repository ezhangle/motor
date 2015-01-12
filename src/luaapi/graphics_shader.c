#include <string.h>
#include <stdlib.h>
#include <lauxlib.h>
#include "tools.h"
#include "graphics_shader.h"
#include "../graphics/shader.h"
#include "../3rdparty/slre/slre.h"
#include "../filesystem/filesystem.h"

static struct {
  int shaderMT;
  int currentShaderRef;
  struct slre fragmentSingleShaderDetectRegex;
  struct slre vertexShaderDetectRegex;
} moduleData;

static char const * fragmentSingleShaderDetectRegexSrc = "vec4\\s*effect\\s*\\(";
static char const * vertexShaderDetectRegexSrc = "vec4\\s*position\\s*\\(";


bool static isVertexShader(char const* str) {
  return slre_match(&moduleData.vertexShaderDetectRegex, str, strlen(str), NULL);
}

bool static isSingleFragmentShader(char const* str) {
  return slre_match(&moduleData.fragmentSingleShaderDetectRegex, str, strlen(str), NULL);
}

int l_graphics_newShader(lua_State* state) {
  char const* vertexSrc = l_tools_tostring_or_err(state, 1);
  char const* fragmentSrc = NULL;
  char * loadedFile1 = NULL;
  char * loadedFile2 = NULL;

  if(lua_isstring(state, 2)) {
    fragmentSrc = lua_tostring(state, 2);
    
    if(!isVertexShader(vertexSrc)) {
      int loadedFile1Size = filesystem_read(vertexSrc, &loadedFile1);
      if(!loadedFile1 || !isVertexShader(loadedFile1)) {
        free(loadedFile1);
        lua_pushstring(state, "input 1 is not a valid vertex shader");
        return lua_error(state);
      }
      vertexSrc = loadedFile1;
    }

    if(!isSingleFragmentShader(fragmentSrc)) {
      int loadedFile2Size = filesystem_read(fragmentSrc, &loadedFile2);
      if(!loadedFile2 || !isSingleFragmentShader(loadedFile2)) {
        free(loadedFile1);
        free(loadedFile2);
        lua_pushstring(state, "input 2 is not a valid fragment shader");
        return lua_error(state);
      }
      fragmentSrc = loadedFile2;
    }

  } else {
    if(isVertexShader(vertexSrc)) {
      // nothing required
    } else if(isSingleFragmentShader(vertexSrc)) {
      fragmentSrc = vertexSrc;
      vertexSrc = NULL;
    } else {
      
      int loadedFile1Size = filesystem_read(vertexSrc, &loadedFile1);
      if(!loadedFile1) {
        lua_pushstring(state, "could not open file");
        return lua_error(state);
      }

      if(isSingleFragmentShader(loadedFile1)) {
        fragmentSrc = loadedFile1;
        vertexSrc = NULL;
      } else if(isVertexShader(loadedFile1)) {
        vertexSrc = loadedFile1;
        fragmentSrc = NULL;
      } else {
        free(loadedFile1);
        lua_pushstring(state, "input is not a valid shader");
        return lua_error(state);
      }
    }
  }

  l_graphics_Shader * shader = lua_newuserdata(state, sizeof(l_graphics_Shader));
  graphics_Shader_new(&shader->shader, vertexSrc, fragmentSrc);

  lua_rawgeti(state, LUA_REGISTRYINDEX, moduleData.shaderMT);
  lua_setmetatable(state, -2);

  free(loadedFile1);
  free(loadedFile2);

  return 1;
}

l_check_type_fn(l_graphics_isShader, moduleData.shaderMT)
l_to_type_fn(l_graphics_toShader, l_graphics_Shader)

int l_graphics_gcShader(lua_State* state) {
  l_graphics_Shader *shader = l_graphics_toShader(state, 1);
  graphics_Shader_free(&shader->shader);
  return 0;
}

int l_graphics_setShader(lua_State *state) {
  if(lua_isnoneornil(state, 1)) {
    graphics_setDefaultShader();
    luaL_unref(state, LUA_REGISTRYINDEX, moduleData.currentShaderRef);
    moduleData.currentShaderRef = LUA_NOREF;
    return 0;
  } else if(l_graphics_isShader(state, 1)) {
    l_graphics_Shader* shader = l_graphics_toShader(state, 1);
    luaL_unref(state, LUA_REGISTRYINDEX, moduleData.currentShaderRef);
    lua_settop(state, 1);
    moduleData.currentShaderRef = luaL_ref(state, LUA_REGISTRYINDEX);
    graphics_setShader(&shader->shader);
    return 0;
  } else {
    lua_pushstring(state, "expected nil or shader");
    return lua_error(state);
  }
}

static luaL_Reg const shaderMetatableFuncs[] = {
  {"__gc",   l_graphics_gcShader},
  {NULL, NULL}
};

static luaL_Reg const shaderFreeFuncs[] = {
  {"newShader", l_graphics_newShader},
  {"setShader", l_graphics_setShader},
  {NULL, NULL}
};

void l_graphics_shader_register(lua_State *state) {
  moduleData.currentShaderRef = LUA_NOREF;
  l_tools_register_funcs_in_module(state, "graphics", shaderFreeFuncs);
  moduleData.shaderMT  = l_tools_make_type_mt(state, shaderMetatableFuncs);

  slre_compile(&moduleData.fragmentSingleShaderDetectRegex, fragmentSingleShaderDetectRegexSrc);
  slre_compile(&moduleData.vertexShaderDetectRegex, vertexShaderDetectRegexSrc);
}
