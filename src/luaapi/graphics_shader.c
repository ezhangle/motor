#include <string.h>
#include <stdlib.h>
#include <lauxlib.h>
#include "tools.h"
#include "graphics_shader.h"
#include "graphics_image.h"
#include "graphics_canvas.h"
#include "../graphics/shader.h"
#include "../3rdparty/slre/slre.h"
#include "../filesystem/filesystem.h"
#include "../math/minmax.h"

static struct {
  int shaderMT;
  int currentShaderRef;
  struct slre fragmentSingleShaderDetectRegex;
  struct slre vertexShaderDetectRegex;
  void * sendValueBuffer;
  int sendValueBufferSize;
} moduleData;

static char const * fragmentSingleShaderDetectRegexSrc = "vec4\\s*effect\\s*\\(";
static char const * vertexShaderDetectRegexSrc = "vec4\\s*position\\s*\\(";


bool static isVertexShader(char const* str) {
  return slre_match(&moduleData.vertexShaderDetectRegex, str, strlen(str), NULL);
}

bool static isSingleFragmentShader(char const* str) {
  return slre_match(&moduleData.fragmentSingleShaderDetectRegex, str, strlen(str), NULL);
}

int static l_graphics_newShader(lua_State* state) {
  char const* vertexSrc = l_tools_toStringOrError(state, 1);
  char const* fragmentSrc = NULL;
  char * loadedFile1 = NULL;
  char * loadedFile2 = NULL;

  if(lua_isstring(state, 2)) {
    fragmentSrc = lua_tostring(state, 2);
    
    if(!isVertexShader(vertexSrc)) {
      // TODO
      int loadedFile1Size = filesystem_read(vertexSrc, &loadedFile1);
      (void) loadedFile1Size;
      if(!loadedFile1 || !isVertexShader(loadedFile1)) {
        free(loadedFile1);
        lua_pushstring(state, "input 1 is not a valid vertex shader");
        return lua_error(state);
      }
      vertexSrc = loadedFile1;
    }

    if(!isSingleFragmentShader(fragmentSrc)) {
      // TODO
      int loadedFile2Size = filesystem_read(fragmentSrc, &loadedFile2);
      (void)loadedFile2Size;

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
      // TODO
      int loadedFile1Size = filesystem_read(vertexSrc, &loadedFile1);
      (void) loadedFile1Size;
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

l_checkTypeFn(l_graphics_isShader, moduleData.shaderMT)
l_toTypeFn(l_graphics_toShader, l_graphics_Shader)

static int l_graphics_gcShader(lua_State* state) {
  l_graphics_Shader *shader = l_graphics_toShader(state, 1);
  graphics_Shader_free(&shader->shader);
  return 0;
}

static int l_graphics_setShader(lua_State *state) {
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

static void growBuffer(int size) {
  if(size > moduleData.sendValueBufferSize) {
    free(moduleData.sendValueBuffer);
    moduleData.sendValueBuffer = malloc(size);
    moduleData.sendValueBufferSize = size;
  }
}

#define mkScalarSendFunc(name, type, totypefunc) \
  static void name(lua_State *state, l_graphics_Shader* shader, graphics_ShaderUniformInfo const* info) { \
    int count = min(lua_gettop(state) - 2, info->elements);         \
                                                                    \
    growBuffer(sizeof(type) * count);                               \
    type * numbers = (type*)moduleData.sendValueBuffer;             \
                                                                    \
    for(int i = 0; i < count; ++i) {                                \
      numbers[i] = totypefunc(state, 3 + i);                        \
    }                                                               \
                                                                    \
    graphics_Shader_ ## name(&shader->shader, info, count, numbers);\
  }

mkScalarSendFunc(sendIntegers, GLint,   l_tools_toNumberOrError)
mkScalarSendFunc(sendFloats,   GLfloat, l_tools_toNumberOrError)
mkScalarSendFunc(sendBooleans, GLint,   l_tools_toBooleanOrError)

#undef mkScalarSendFunc

#define mkVectorSendFunc(name, valuetype, totypefunc) \
  static void name(lua_State *state, l_graphics_Shader *shader, graphics_ShaderUniformInfo const* info) { \
    int components = graphics_shader_toMotorComponents(info->type); \
    int count = min(lua_gettop(state) - 2, info->elements);         \
                                                                    \
    growBuffer(sizeof(valuetype) * count * components);             \
    valuetype* numbers = (valuetype*)moduleData.sendValueBuffer;    \
                                                                    \
    for(int i = 0; i < count; ++i) {                                \
      for(int j = 0; j < components; ++j) {                         \
        lua_rawgeti(state, i + 3, j + 1);                           \
        numbers[i*components+j] = totypefunc(state, -1);            \
      }                                                             \
    }                                                               \
                                                                    \
    graphics_Shader_ ## name(&shader->shader, info, count, numbers);\
  }

mkVectorSendFunc(sendIntegerVectors, GLint,   l_tools_toNumberOrError)
mkVectorSendFunc(sendFloatVectors,   GLfloat, l_tools_toNumberOrError)
mkVectorSendFunc(sendBooleanVectors, GLint,   l_tools_toBooleanOrError)

#undef mkVectorSendFunc

static void sendFloatMatrices(lua_State *state, l_graphics_Shader* shader, graphics_ShaderUniformInfo const* info) {
  int components = graphics_shader_toMotorComponents(info->type);
  int count = min(lua_gettop(state) - 2, info->elements);
  growBuffer(sizeof(float) * components * components * count);

  float* numbers = (float*)moduleData.sendValueBuffer;

  for(int i = 0; i < count; ++i) {
    for(int j = 0; j < components; ++j) {
      for(int k = 0; k < components; ++k) {
        lua_rawgeti(state, i + 3, j + 1);
        numbers[i*components+j] = totypefunc(state, -1);
      }
    }
  }

}

static void sendSamplers(lua_State *state, l_graphics_Shader* shader, graphics_ShaderUniformInfo const* info) {

}

static int l_graphics_Shader_send(lua_State *state) {
  l_assertType(state, 1, l_graphics_isShader); 
  l_graphics_Shader* shader = l_graphics_toShader(state, 1);

  char const* name = l_tools_toStringOrError(state, 2);

  graphics_ShaderUniformInfo const* info = graphics_Shader_getUniform(&shader->shader, name);

  switch(info->type) {
  case GL_INT:
    sendIntegers(state, shader, info);
    break;

  case GL_FLOAT:
    sendFloats(state, shader, info);
    break;

  case GL_BOOL:
    sendBooleans(state, shader, info);
    break;

  case GL_INT_VEC2:
  case GL_INT_VEC3:
  case GL_INT_VEC4:
    sendIntegerVectors(state, shader, info);
    break;

  case GL_FLOAT_VEC2:
  case GL_FLOAT_VEC3:
  case GL_FLOAT_VEC4:
    sendFloatVectors(state, shader, info);
    break;

  case GL_BOOL_VEC2:
  case GL_BOOL_VEC3:
  case GL_BOOL_VEC4:
    sendBooleanVectors(state, shader, info);
    break;

  case GL_FLOAT_MAT2:
  case GL_FLOAT_MAT3:
  case GL_FLOAT_MAT4:
    sendFloatMatrices(state, shader, info);
    break;

  case GL_SAMPLER_2D:
    sendSamplers(state, shader, info);
    break;

  };

#if 0
  graphics_ShaderUniformType type = graphics_shader_toMotorType(info);

  switch(type) {
  case graphics_ShaderUniformType_int:
    sendInteger(state, shader, info);
    break;

  case graphics_ShaderUniformType_float:
    sendFloat(state, shader, info);
    break;

  case graphics_ShaderUniformType_bool:
    sendBoolean(state, shader, info);
    break;

  case graphics_ShaderUniformType_sampler:
    sendSampler(state, shader, info);
    break;
  }
#endif

#if 0
  int type = lua_type(state, 3);
  switch(type) {
  case LUA_TNUMBER:
    sendNumbers(&shader->shader, name, state);
    break;
  case LUA_TBOOLEAN:
    sendBooleans(&shader->shader, name, state);
    break;
  case LUA_TTABLE:
    {
      lua_rawgeti(state, 3, 1);
      int type2 = lua_type(state, 3);
      lua_pop(state, 1);
      switch(type2) {
      case LUA_TNUMBER:
        sendVectors(&shader->shader, name, state);
        break;
      case LUA_TTABLE:
        sendMatrices(&shader->shader, name, state);
        break;
      default:
        // TODO error
        break;
      };
    }
    break;
  case LUA_TUSERDATA:
    if(l_graphics_isImage(state, 3)) {
      sendImage(&shader->shader, name, state);
    } else if(l_graphics_isCanvas(state, 3)) {
      sendCanvas(&shader->shader, name, state);
    } else {
      // TODO error
    }
    break;
  default:
    // TODO error
    break;
  };
#endif
  return 0;
}

static const l_tools_Enum l_graphics_ShaderUniformType[] = {
  {"int",   graphics_ShaderUniformType_int},
  {"float", graphics_ShaderUniformType_float},
  {"bool",  graphics_ShaderUniformType_bool},
  {"image", graphics_ShaderUniformType_sampler}
};

static int l_graphics_Shader_getExternVariable(lua_State* state) {
  l_assertType(state, 1, l_graphics_isShader); 
  l_graphics_Shader const* shader = l_graphics_toShader(state, 1);

  char const* name = l_tools_toStringOrError(state, 2);

  graphics_ShaderUniformInfo const* info = graphics_Shader_getUniform(&shader->shader, name);

  if(!info) {
    goto errout;
  }
  
  graphics_ShaderUniformType type =  graphics_shader_toMotorType(info->type);
  if(type == graphics_ShaderUniformType_none) {
    goto errout;
  }

  l_tools_pushEnum(state, type, l_graphics_ShaderUniformType);
  lua_pushnumber(state, graphics_shader_toMotorComponents(info->type));
  lua_pushnumber(state, info->elements);

  return 3;

errout:
  lua_pushnil(state);
  lua_pushnil(state);
  lua_pushnil(state);
  return 3;
}


static luaL_Reg const shaderMetatableFuncs[] = {
  {"__gc",              l_graphics_gcShader},
  {"send",              l_graphics_Shader_send},
  {"getExternVariable", l_graphics_Shader_getExternVariable},
  {NULL, NULL}
};

static luaL_Reg const shaderFreeFuncs[] = {
  {"newShader", l_graphics_newShader},
  {"setShader", l_graphics_setShader},
  {NULL, NULL}
};

void l_graphics_shader_register(lua_State *state) {
  moduleData.currentShaderRef = LUA_NOREF;
  l_tools_registerFuncsInModule(state, "graphics", shaderFreeFuncs);
  moduleData.shaderMT  = l_tools_makeTypeMetatable(state, shaderMetatableFuncs);

  slre_compile(&moduleData.fragmentSingleShaderDetectRegex, fragmentSingleShaderDetectRegexSrc);
  slre_compile(&moduleData.vertexShaderDetectRegex, vertexShaderDetectRegexSrc);
}
