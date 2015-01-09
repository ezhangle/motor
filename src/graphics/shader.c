#include "shader.h"

GLchar const *defaultVertexSource = 
  "uniform   mat4 transform;\n"  
  "uniform   mat4 projection;\n"
  "uniform   mat2 textureRect;\n"
  "attribute vec2 vPos;\n"
  "attribute vec2 vUV;\n"
  "attribute vec4 vColor;\n"
  "varying   vec2 fUV;\n"
  "varying   vec4 fColor;\n"
  "void main() {\n"
  "  gl_Position = projection * transform * vec4(vPos, 1.0, 1.0);\n"
  "  fUV = vUV * textureRect[1] + textureRect[0];\n"
  "  fColor = vColor;\n"
  "}\n";

GLchar const *defaultFragmentSource =
  "precision mediump float;\n"
  "varying vec2 fUV;\n"
  "varying vec4 fColor;\n"
  "uniform sampler2D tex;\n"
  "uniform vec4 color;\n"
  "void main() {\n"
  "  gl_FragColor = texture2D(tex, fUV) * color * fColor;\n"
  "}\n";

void graphics_Shader_new(graphics_Shader *shader, char const* vertexCode, char const* fragmentCode) {
  if(!vertexCode) {
    vertexCode = defaultVertexSource;
  }

  if(!fragmentCode) {
    fragmentCode = defaultFragmentSource;
  }

  shader->program = glCreateProgram();
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShader, 1, &vertexCode, 0);
  glCompileShader(vertexShader);
  glAttachShader(shader->program, vertexShader);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentCode, 0);
  glCompileShader(fragmentShader);
  glAttachShader(shader->program, fragmentShader);

  glBindAttribLocation(shader->program, 0, "vPos");
  glBindAttribLocation(shader->program, 1, "vUV");
  glBindAttribLocation(shader->program, 2, "vColor");
  glLinkProgram(shader->program);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  shader->uniformLocations.transform   = glGetUniformLocation(shader->program, "transform");
  shader->uniformLocations.projection  = glGetUniformLocation(shader->program, "projection");
  shader->uniformLocations.textureRect = glGetUniformLocation(shader->program, "textureRect");
  shader->uniformLocations.tex         = glGetUniformLocation(shader->program, "tex");
  shader->uniformLocations.color       = glGetUniformLocation(shader->program, "color");
}

void graphics_Shader_activate(graphics_Shader *shader, mat4x4 const* projection, mat4x4 const* transform, graphics_Quad const* textureRect, float const* useColor) {

  glUseProgram(shader->program);

  glUniform1i(       shader->uniformLocations.tex,               0);
  glUniformMatrix4fv(shader->uniformLocations.projection,  1, 0, (GLfloat const*)projection);
  glUniformMatrix2fv(shader->uniformLocations.textureRect, 1, 0, (GLfloat const*)textureRect);
  glUniform4fv(      shader->uniformLocations.color,       1,                    useColor);
  glUniformMatrix4fv(shader->uniformLocations.transform,   1, 0, (GLfloat const*)transform);
}
