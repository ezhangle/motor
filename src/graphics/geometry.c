#include <tgmath.h>
#include <stdlib.h>
#include "geometry.h"
#include "graphics.h"
#include "shader.h"
#include "matrixstack.h"

static struct {
  GLuint dataVBO;
  GLuint dataIBO;
  GLuint dataVAO;
  int currentDataSize;
  int currentIndexSize;
  float *data;
  uint16_t *index;
  graphics_Shader plainColorShader;
} moduleData;

void graphics_geometry_init() {
  glGenVertexArrays(1, &moduleData.dataVAO);
  glBindVertexArray(moduleData.dataVAO);
  glGenBuffers(1, &moduleData.dataIBO);
  glGenBuffers(1, &moduleData.dataVBO);
  glBindBuffer(GL_ARRAY_BUFFER, moduleData.dataVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moduleData.dataIBO);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(2*sizeof(float)));

  graphics_Shader_new(&moduleData.plainColorShader, NULL,
    "vec4 effect(vec4 color, Image texture, vec2 texture_coords, vec2 screen_cords ) {\n"
    "  return color;\n"
    "}\n");
}

static void updateBuffers(int vertices, int indices) {

}

void graphics_geometry_fill_circle(float x, float y, float radius, int segments) {
  int datasize = (segments+1) * 6 * sizeof(float);
  if(moduleData.currentDataSize < datasize) {
    free(moduleData.data);
    moduleData.data = (float*)malloc(datasize);
    moduleData.currentDataSize = datasize;
  }

  int indexsize = (segments + 2) * sizeof(uint16_t);
  if(moduleData.currentIndexSize < indexsize) {
    free(moduleData.index);
    moduleData.index = (uint16_t*)malloc(indexsize);
    moduleData.currentIndexSize = indexsize;
  }


  float step = 2*M_PI / segments;
  float ang = 0;
  moduleData.data[0] = x;
  moduleData.data[1] = y;
  moduleData.data[2] = 1.0f;
  moduleData.data[3] = 1.0f;
  moduleData.data[4] = 1.0f;
  moduleData.data[5] = 1.0f;
  moduleData.index[0] = 0;
  moduleData.index[segments+1] = 1;
  for(int i = 0; i < segments; ++i, ang -= step) {
    moduleData.data[6*(i+1)  ] = sin(ang) * radius + x;
    moduleData.data[6*(i+1)+1] = cos(ang) * radius + y;
    moduleData.data[6*(i+1)+2] = 1.0f;
    moduleData.data[6*(i+1)+3] = 1.0f;
    moduleData.data[6*(i+1)+4] = 1.0f;
    moduleData.data[6*(i+1)+5] = 1.0f;
    moduleData.index[i+1] = i+1;
  }
  glBindBuffer(GL_ARRAY_BUFFER, moduleData.dataVBO);
  glBufferData(GL_ARRAY_BUFFER, datasize, moduleData.data, GL_STREAM_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moduleData.dataIBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexsize, moduleData.index, GL_STREAM_DRAW);

  
  graphics_Shader *shader = graphics_getShader();
  graphics_setShader(&moduleData.plainColorShader);
  mat4x4 tr;
  m4x4_new_identity(&tr);
  graphics_Quad quad = {0,0,1,1};

  graphics_drawArray(&quad, &tr, moduleData.dataVAO, moduleData.dataIBO, segments+2, GL_TRIANGLE_FAN, GL_UNSIGNED_SHORT, graphics_getColorPtr(), 1, 1);

  graphics_setShader(shader);
}

void graphics_geometry_fill_rectangle(float x, float y, float w, float h) {
  graphics_Shader *shader = graphics_getShader();

  graphics_Image img = {
    0, w, h
  };
  graphics_Quad q = {0,0,1,1};
  graphics_setShader(&moduleData.plainColorShader);
  graphics_Image_draw(&img, &q, x, y, 0, 1, 1, 0, 0, 0, 0);

  graphics_setShader(shader);
}
