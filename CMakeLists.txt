cmake_minimum_required(VERSION 3.25)
project(2_3 C)

set(CMAKE_C_STANDARD 11)

add_executable(2_3 main.c graph_create.h graph_draw.h X_routines.h)
target_link_libraries(2_3 X11)
target_link_libraries(2_3 m)
