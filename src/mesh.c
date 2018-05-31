#include "mesh.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

mesh_t* mesh_new(const char* filename)
{
  mesh_t* mesh = malloc(sizeof(*mesh));
  if(!mesh) return 0;
  memset(mesh, 0, sizeof(*mesh));
  FILE* f = fopen(filename, "rb");
  fscanf(f, "%d", &mesh->v_count);
  mesh->vertices = malloc(sizeof(*mesh)*mesh->v_count);
  for(int i = 0; i < mesh->v_count; ++i)
    fscanf(f, "%f", &mesh->vertices[i]);
  glGenBuffers(1, &mesh->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(*mesh->vertices)*mesh->v_count, mesh->vertices, GL_STATIC_DRAW);
  return mesh;
}

void mesh_free(mesh_t* mesh)
{
  if(!mesh) return;
  if(mesh->vertices) free(mesh->vertices);
  free(mesh);
}

