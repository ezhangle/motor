#include "batch.h"
#include "graphics.h"

static struct {
  GLuint sharedIndexBuffer;
  uint16_t *sharedIndexBufferData;
  int indexBufferSize;
} moduleData;


static void graphics_batch_makeIndexBuffer(int quadCount) {
  if(quadCount <= moduleData.indexBufferSize) {
    return;
  }

  // Round up to multiple of 128
  quadCount += (quadCount % 128 == 0)?0:(128 - quadCount % 128);

  moduleData.sharedIndexBufferData = realloc(moduleData.sharedIndexBufferData, quadCount * 6 * sizeof(uint16_t));
  for(int i = moduleData.indexBufferSize; i < quadCount; ++i) {
    moduleData.sharedIndexBufferData[6*i] = 4*i;
    moduleData.sharedIndexBufferData[6*i+1] = 4*i+1;
    moduleData.sharedIndexBufferData[6*i+2] = 4*i+2;
    moduleData.sharedIndexBufferData[6*i+3] = 4*i+2;
    moduleData.sharedIndexBufferData[6*i+4] = 4*i+1;
    moduleData.sharedIndexBufferData[6*i+5] = 4*i+3;
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moduleData.sharedIndexBuffer);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*quadCount, moduleData.sharedIndexBufferData, GL_STATIC_DRAW);
  moduleData.indexBufferSize = quadCount;
}

void graphics_batch_init() {
  glGenBuffers(1, &moduleData.sharedIndexBuffer);
  moduleData.sharedIndexBufferData = NULL; 
  moduleData.indexBufferSize = 0;
  graphics_batch_makeIndexBuffer(128);
}

void graphics_Batch_new(graphics_Batch* batch, graphics_Image const* texture, int maxSize, graphics_BatchUsage usage) {
  batch->texture = texture;
  glGenVertexArrays(1, &batch->vao);
  glBindVertexArray(batch->vao);
  glGenBuffers(1, &batch->vbo);
  batch->vertexData = calloc(4*maxSize, sizeof(graphics_Vertex));
  glBindBuffer(GL_ARRAY_BUFFER, batch->vbo);
  glBufferData(GL_ARRAY_BUFFER, 4*maxSize*sizeof(graphics_Vertex), batch->vertexData, usage);
  batch->maxCount = maxSize;
  batch->insertPos = 0;

  graphics_batch_makeIndexBuffer(maxSize);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(graphics_Vertex), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(graphics_Vertex), (GLvoid const*)(2*sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(graphics_Vertex), (GLvoid const*)(4*sizeof(float)));

  batch->dirty = false;
  batch->bound = false;
  batch->usage = usage;
}

void graphics_Batch_free(graphics_Batch* batch) {
  // TODO
}

static const vec2 batchQuadPts[4] = {
  {0,0},{0,1},{1,0},{1,1}
};

void graphics_Batch_add(graphics_Batch* batch, graphics_Quad const* q, float x, float y, float r, float sx, float sy, float ox, float oy, float kx, float ky) {

  if(batch->insertPos == batch->maxCount) {
    return;
  }

  mat3x3 transform;
  m3x3_new_transform2d(&transform, x, y, r, sx, sy, ox, oy, kx, ky, q->w * batch->texture->width, q->h * batch->texture->height);

  graphics_Vertex *v = batch->vertexData+ 4*batch->insertPos;
  
  for(int i = 0; i < 4; ++i) {
    m3x3_mul_v2(&v[i].pos, &transform, batchQuadPts+i);
    float* c = (float*)(&v[i].color);
    for(int j = 0; j < 4; ++j) {
      c[j] = 1.0f;
    }
  }

  v[0].uv.x = q->x;
  v[0].uv.y = q->y;
  v[1].uv.x = q->x;
  v[1].uv.y = q->y + q->h;
  v[2].uv.x = q->x + q->w;
  v[2].uv.y = q->y;
  v[3].uv.x = q->x + q->w;
  v[3].uv.y = q->y + q->h;

  if(batch->bound) {
    batch->dirty = true;
  } else {
    glBindBuffer(GL_ARRAY_BUFFER, batch->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, batch->insertPos * 4, 4*sizeof(graphics_Vertex), &v);
  }

  ++batch->insertPos;
}

static const graphics_Quad fullQuad = {
  0.0f,0.0f,1.0f,1.0f
};
void graphics_Batch_draw(graphics_Batch const* batch,
                         float x, float y, float r, float sx, float sy,
                         float ox, float oy, float kx, float ky) {

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, batch->texture->texID);
  mat4x4 tr2d;
  m4x4_new_transform2d(&tr2d, x, y, r, sx, sy, ox, oy, kx, ky, 1, 1);
  graphics_drawArray(&fullQuad, &tr2d, batch->vao, moduleData.sharedIndexBuffer, batch->insertPos*6, GL_TRIANGLES, GL_UNSIGNED_SHORT);
}

void graphics_Batch_bind(graphics_Batch *batch) {
  batch->bound = true;
}

void graphics_Batch_unbind(graphics_Batch *batch) {
  if(!batch->bound) {
    return;
  }

  if(batch->dirty) {
    batch->dirty = false;

    glBindBuffer(GL_ARRAY_BUFFER, batch->vbo);
    glBufferData(GL_ARRAY_BUFFER, 4*batch->maxCount*sizeof(graphics_Vertex), NULL, batch->usage);
    glBufferData(GL_ARRAY_BUFFER, 4*batch->maxCount*sizeof(graphics_Vertex), batch->vertexData, batch->usage);
    
  }
  batch->bound = false;
}
