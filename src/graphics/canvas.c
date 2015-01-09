#include "canvas.h"
#include "image.h"

void graphics_Canvas_new(graphics_Canvas *canvas, int width, int height) {
  GLuint oldTex, oldFBO;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&oldTex);
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&oldFBO);
  glGenTextures(1, &canvas->texture);
  glBindTexture(GL_TEXTURE_2D, canvas->texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  glGenFramebuffers(1, &canvas->fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, canvas->fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, canvas->texture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
  glBindTexture(GL_TEXTURE_2D, oldTex);

  m4x4_new_translation(&canvas->projectionMatrix, -1.0f, -1.0f, 0.0f);
  m4x4_scale(&canvas->projectionMatrix, 2.0f / width, 2.0f / height, 0.0f);
  canvas->width = width;
  canvas->height = height;
}

void graphics_Canvas_free(graphics_Canvas *canvas) {
  glDeleteTextures(1, &canvas->texture);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDeleteFramebuffers(1, &canvas->fbo);
}

void graphics_Canvas_draw(graphics_Canvas const* canvas, graphics_Quad const* quad,
                         float x, float y, float r, float sx, float sy,
                         float ox, float oy, float kx, float ky) {
  graphics_Image img = {NULL, canvas->texture, canvas->width, canvas->height};

  graphics_Image_draw(&img, quad, x, y, r, sx, sy, ox, oy, kx, ky);
  
}
