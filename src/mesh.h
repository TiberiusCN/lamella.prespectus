#pragma once

#include <GL/gl.h>

typedef struct
{
  float* vertices;
  int v_count;
  GLuint vbo;
} mesh_t;

mesh_t* mesh_new(const char* filename);
void mesh_free(mesh_t* mesh);
