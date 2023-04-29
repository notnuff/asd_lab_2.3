#ifndef INC_2_3_GRAPH_DRAW_H
#define INC_2_3_GRAPH_DRAW_H

#include <math.h>
#include <stdlib.h>
#include "consts_n_structures.h"
#include "X_routines.h"
#include "vec_math_lib.h"

void draw_graph_vertices (point_t *graph, int size);

void arrow(double theta, int px, int py);

void draw_arc (vec_4 *vector, int offset_radius, int with_arrow);

void draw_line (vec_4 *vector, int with_arrow);

void draw_graph_u (point_t *graph, int **relation_matrix, int size);

void draw_graph_o (point_t *graph, int **relation_matrix, int size);

#endif //INC_2_3_GRAPH_DRAW_H
