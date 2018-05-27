#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>

#include "shaders.h"
#include "mesh.h"

char gFifoBuf[2048];
SDL_Window* gWnd;
SDL_GLContext gContext;
int sdlState = SS_EMPTY;

int sdl_init()
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);

  gWnd = SDL_CreateWindow("Prespectus", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  if(!gWnd)
  {
    printf("Error: %s\n", SDL_GetError());
    return -1;
  }

  SDL_GLContext tcon = SDL_GL_CreateContext(gWnd);
  if(!tcon)
  {
    printf("Error: %s\n", SDL_GetError());
    return -2;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

  gContext = SDL_GL_CreateContext(gWnd);

  if(!gContext)
  {
    printf("Error: %s\n", SDL_GetError());
    return -3;
  }

  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 24);

  int res = glewInit();
  if(GLEW_OK != res)
  {
    printf("Error: GLEW failed: %s\n", glewGetErrorString(res));
    return -4;
  }

  SDL_GL_MakeCurrent(gWnd, gContext);
  glClearColor(1,0,1,1);
  SDL_GL_MakeCurrent(gWnd, 0);

  return 0;
}

int sdl_loop()
{
  SDL_Event ev;
  if(SDL_PollEvent(&ev))
  {
    if(ev.type == SDL_QUIT) return -1;
  }
  SDL_GL_MakeCurrent(gWnd, gContext);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(gWnd);
  SDL_GL_MakeCurrent(gWnd, 0);

  return 0;
}

void sdl_free()
{
  SDL_GL_DeleteContext(gContext);
  SDL_DestroyWindow(gWnd);
  SDL_Quit();
}

int main(int argc, char** argv)
{
  int res, fd;
  res = mkdir("/tmp/lamella", 0777);
  res = mkfifo("/tmp/lamella/prespectus", 0777);
  fd = open("/tmp/lamella/prespectus", O_RDONLY);
  if(fd <= 0)
  {
    printf("Starting failed!\n");
    return -1;
  }
  printf("Ready!\n");
  int reg = 0;
  while(1)
  {
    res = read(fd, gFifoBuf, 2047);
    if(res < 0)
    {
      close(fd);
      remove("/tmp/lamella/prespectus");
      return -2;
    }
    gFifoBuf[res] = 0;
    const char* seeker = gFifoBuf;
    while(*seeker)
    {
      if(res)
      {
        if(!strncmp(seeker, "exit!", 5)) goto exitmark;
        else if(!strncmp(seeker, "start", 5))
        {
          switch(sdlState)
          {
          case SS_EMPTY:
            if(!sdl_init())
            {
              if(res = shaders_init())
              {
                shaders_error_print(res);
                sdl_free();
              } else sdlState = SS_LOOP;
            }
            break;
          case SS_LOOP:
            printf("SDL already exists!\n");
            break;
          }
        } else
        if(!strncmp(seeker, "stop", 4))
        {
          switch(sdlState)
          {
          case SS_EMPTY:
            printf("SDL is empty!\n");
            break;
          case SS_LOOP:
            sdl_free();
            sdlState = SS_EMPTY;
            break;
          }
        } else
        if(!strncmp(seeker, "shader ", 7))
        {
          seeker += 7;
          SDL_GL_MakeCurrent(gWnd, gContext);
          switch(sdlState)
          {
          case SS_EMPTY:
            printf("SDL is empty!\n");
            break;
          case SS_LOOP:
            if(res = shaders_command(seeker)) shaders_error_print(res);
            break;
          }
          SDL_GL_MakeCurrent(gWnd, 0);
        } else {
          printf("unrecognized!\n");
          break;
        }
      }
      while(*seeker != '\n') seeker++;
      while(*seeker == '\n') seeker++;
    }
    if(sdlState == SS_LOOP)
    {
      if(sdl_loop()) sdlState = SS_EMPTY;
    }
  }
exitmark:
  if(sdlState != SS_EMPTY) sdl_free();
  close(fd);
  remove("/tmp/lamella/prespectus");
  return 0;
}
