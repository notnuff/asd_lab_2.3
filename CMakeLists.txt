cmake_minimum_required(VERSION 3.25)
project(2_3 C)

set(CMAKE_C_STANDARD 11)

add_executable(2_3 main.c src/graph_create.h src/graph_draw.h src/X_routines.h src/consts_n_structures.h "src/c files/graph_create.c" src/vec_math_lib.h "src/c files/vec_math_lib.c" "src/c files/X_routines.c" "src/c files/graph_draw.c")
target_link_libraries(2_3 X11)
target_link_libraries(2_3 m)
