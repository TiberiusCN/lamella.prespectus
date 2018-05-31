#include "shaders.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/gl.h>

typedef struct
{
  char* name;
  GLuint shader;
} shader_t;

typedef struct
{
  char* name;
  GLuint program;
} shaders_t;

#define dMaxShaders 32
#define dMaxPrograms 128

int cSh = 0;
shader_t* sh[dMaxShaders];
int cProg = 0;
shaders_t* prog[dMaxPrograms];

int shader_find(const char* name)
{
  for(int i = 0; i < cSh; ++i)
  {
    if(!strncmp(sh[i]->name, name, strlen(name)+1)) return i;
  }
  return -1;
}

int program_find(const char* name)
{
  for(int i = 0; i < cProg; ++i)
  {
    if(!strncmp(prog[i]->name, name, strlen(name)+1)) return i;
  }
  return -1;
}

int shader_new(const char* name, const char* filename, int type)
{
  if(shader_find(name) != -1) return E_SHADERS_EXISTS;
  switch(type)
  {
  case 0: type = GL_VERTEX_SHADER; break;
  case 1: type = GL_FRAGMENT_SHADER; break;
  case 2: type = GL_GEOMETRY_SHADER; break;
  default: return E_SHADERS_UNKNOWN_TYPE;
  }
  if(cSh == dMaxShaders) return E_SHADERS_MAX;
  char* src;
  FILE* f = fopen(filename, "rb");
  if(!f) return E_SHADERS_NO_SUCH_FILE;
  fseek(f, 0, SEEK_END);
  int len = ftell(f);
  fseek(f, 0, SEEK_SET);
  src = malloc(len+1);
  fread(src, len, 1, f);
  fclose(f);
  src[len] = 0;
  shader_t* shader = malloc(sizeof(*shader));
  if(!shader) return E_SHADERS_MEMORY;
  memset(shader, 0, sizeof(*shader));
  shader->shader = glCreateShader(type);
  glShaderSource(shader->shader, 1, (const GLchar**)&src, &len);
  glCompileShader(shader->shader);
  GLint success;
  GLchar log[512];
  glGetShaderiv(shader->shader, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    glGetShaderInfoLog(shader->shader, 512, 0, log);
    printf("Shader error: %s\n", log);
    return E_SHADERS_COMPILE;
  }
  free(src);
  len = strlen(name)+1;
  shader->name = malloc(len);
  memcpy(shader->name, name, len);
  sh[cSh] = shader;
  cSh++;
  return E_SHADERS_OK;
}

int shader_free(const char* name)
{
  int shn = shader_find(name);
  if(shn == -1) return E_SHADERS_BAD_NAME;
  shader_t* shader = sh[shn];
  glDeleteShader(shader->shader);
  free(shader->name);
  free(shader);
  shader = sh[cSh-1];
  sh[shn] = shader;
  cSh--;
  return E_SHADERS_OK;
}

int program_new(const char* name)
{
  if(program_find(name) != -1) return E_SHADERS_EXISTS;
  if(cProg == dMaxPrograms) return E_SHADERS_MAX;
  shaders_t* program = malloc(sizeof(*program));
  if(!program) return E_SHADERS_MEMORY;
  memset(program, 0, sizeof(*program));
  program->program = glCreateProgram();
  int len = strlen(name)+1;
  program->name = malloc(len);
  memcpy(program->name, name, len);
  prog[cProg] = program;
  cProg++;
  return E_SHADERS_OK;
}

int program_free(const char* name)
{
  int progn = program_find(name);
  if(progn == -1) return E_SHADERS_BAD_NAME;
  shaders_t* program = prog[progn];
  glDeleteProgram(program->program);
  free(program->name);
  free(program);
  program = prog[cProg-1];
  prog[progn] = program;
  cProg--;
  return E_SHADERS_OK;
}

int shaders_lock()
{
}

int shaders_unlock()
{
}

int shaders_free()
{
}

int program_attach(const char* progname, const char* shadername)
{
  int progn = program_find(progname);
  int shan = shader_find(shadername);
  if((progn == -1) || (shan == -1)) return E_SHADERS_BAD_NAME;
  glAttachShader(prog[progn]->program, sh[shan]->shader);
  return E_SHADERS_OK;
}

int program_link(const char* name)
{
  int progn = program_find(name);
  if(progn == -1) return E_SHADERS_BAD_NAME;
  glLinkProgram(prog[progn]->program);
  GLint success;
  GLchar log[512];
  glGetProgramiv(prog[progn]->program, GL_LINK_STATUS, &success);
  if(!success)
  {
    glGetProgramInfoLog(prog[progn]->program, 512, 0, log);
    printf("Link error: %s\n", log);
    return E_SHADERS_LINK;
  }
  return E_SHADERS_OK;
}

int shaders_command(const char* com)
{
  if(!strncmp(com, "shader ", 7))
  {
    com += 7;
    if(!strncmp(com, "compile ", 8))
    {
      com += 8;
      int type;
      char name[512], filename[2048];
      int s = sscanf(com, "%d%s%*[^\"]%*c%[^\"]", &type, name, filename);
      if(s != 3)
      {
        printf("Shaders: expected 3, but %d found in <%s>\n", s, com);
        return E_SHADERS_UNKNOWN_COMMAND;
      }
      return shader_new(name, filename, type);
    } else 
    if(!strncmp(com, "free ", 5))
    {
      com += 5;
      char name[512];
      int s = sscanf(com, "%s", name);
      if(s != 1)
      {
        printf("Shaders: expected 1, but %d found in <%s>\n", s, com);
        return E_SHADERS_UNKNOWN_COMMAND;
      }
      return shader_free(name);
    }
  } else 
  if(!strncmp(com, "program ", 8))
  {
    com += 8;
    if(!strncmp(com, "create ", 7))
    {
      com += 7;
      char name[512];
      int s = sscanf(com, "%s", name);
      if(s != 1)
      {
        printf("Shaders: expected 1, but %d found in <%s>\n", s, com);
        return E_SHADERS_UNKNOWN_COMMAND;
      }
      return program_new(name);
    } else
    if(!strncmp(com, "free ", 5))
    {
      com += 5;
      char name[512];
      int s = sscanf(com, "%s", name);
      if(s != 1)
      {
        printf("Shaders: expected 1, but %d found in <%s>\n", s, com);
        return E_SHADERS_UNKNOWN_COMMAND;
      }
      return program_free(name);
    }
    if(!strncmp(com, "attach ", 7))
    {
      com += 7;
      char progname[512], shadername[512];
      int s = sscanf(com, "%s %s", progname, shadername);
      if(s != 2)
      {
        printf("Shaders: expected 2, but %d found in <%s>\n", s, com);
        return E_SHADERS_UNKNOWN_COMMAND;
      }
      return program_attach(progname, shadername);
    }
    if(!strncmp(com, "link ", 5))
    {
      com += 5;
      char name[512];
      int s = sscanf(com, "%s", name);
      if(s != 1)
      {
        printf("Shaders: expected 1, but %d found in <%s>\n", s, com);
        return E_SHADERS_UNKNOWN_COMMAND;
      }
      return program_link(name);
    }
  }
  return E_SHADERS_UNKNOWN_COMMAND;
}

void shaders_error_print(int error)
{
  switch(error)
  {
  case E_SHADERS_UNKNOWN_COMMAND: printf("Shaders: command not recognized\n"); break;
  case E_SHADERS_OK: printf("Shaders: OK\n"); break;
  case E_SHADERS_BAD_NAME: printf("Shaders: bad name\n"); break;
  case E_SHADERS_COMPILE: printf("Shaders: compile error\n"); break;
  case E_SHADERS_MEMORY: printf("Shaders: not enought memory\n"); break;
  case E_SHADERS_NO_SUCH_FILE: printf("Shaders: file not found\n"); break;
  case E_SHADERS_MAX: printf("Shaders: limit is reached\n"); break;
  case E_SHADERS_LOCKED: printf("Shaders: locked\n"); break;
  case E_SHADERS_LINK: printf("Shaders: link error\n"); break;
  case E_SHADERS_EXISTS: printf("Shaders: already exists\n"); break;
  case E_SHADERS_UNKNOWN_TYPE: printf("Shaders: unknown type\n"); break;
  }
}

int shaders_init()
{
  return E_SHADERS_OK;
}
