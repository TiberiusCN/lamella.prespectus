cmake_minimum_required(VERSION 2.8)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})

pkg_check_modules(GLEW REQUIRED glew)
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})

add_executable(prespectus src/main.c)

add_definitions(${SDL2_CFLAGS_OTHER})
add_definitions(${GLEW_CFLAGS_OTHER})
target_link_libraries(prespectus ${SDL2_LIBRARIES})
target_link_libraries(prespectus ${GLEW_LIBRARIES})
