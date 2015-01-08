#pragma once

#include <SDL_opengl.h>
#include "quad.h"
#include "image.h"
#include "../math/vector.h"
#include "vertex.h"
#include <stdbool.h>

typedef enum {
  graphics_BatchUsage_static = GL_STATIC_DRAW,
  graphics_BatchUsage_dynamic = GL_DYNAMIC_DRAW,
  graphics_BatchUsage_stream = GL_STREAM_DRAW
} graphics_BatchUsage;

typedef struct {
  graphics_Image const *texture;
  GLuint vbo;
  graphics_Vertex *vertexData;
  int maxCount;
  int insertPos;
  GLuint vao;
  bool dirty;
  bool bound;
  graphics_BatchUsage usage;
} graphics_Batch;


void graphics_batch_init();
void graphics_Batch_new(graphics_Batch* batch, graphics_Image const* texture, int maxSize, graphics_BatchUsage usage);
void graphics_Batch_free(graphics_Batch* batch);
void graphics_Batch_add(graphics_Batch* batch, graphics_Quad const* q, float x, float y, float r, float sx, float sy, float ox, float oy, float kx, float ky);
void graphics_Batch_draw(graphics_Batch const* batch, float x, float y, float r, float sx, float sy, float ox, float oy, float kx, float ky);
void graphics_Batch_bind(graphics_Batch *batch);
void graphics_Batch_unbind(graphics_Batch *batch);
