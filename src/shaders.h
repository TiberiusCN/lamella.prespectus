#pragma once

#define E_SHADERS_OK 0
#define E_SHADERS_MAX 1
#define E_SHADERS_COMPILE 2
#define E_SHADERS_LINK 3
#define E_SHADERS_NO_SUCH_FILE 4
#define E_SHADERS_LOCKED 5
#define E_SHADERS_MEMORY 6
#define E_SHADERS_BAD_NAME 7
#define E_SHADERS_UNKNOWN_COMMAND 8
#define E_SHADERS_EXISTS 9
#define E_SHADERS_UNKNOWN_TYPE 10

int shader_new(const char* name, const char* filename, int type);
int shader_free(const char* name);

int program_new(const char* name);
int program_free(const char* name);

int shaders_lock();
int shaders_unlock();
int shaders_free();

int shaders_command(const char* com);
void shaders_error_print(int error);
int shaders_init();
