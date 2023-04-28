#ifndef INC_2_3_GRAPH_CREATE_H
#define INC_2_3_GRAPH_CREATE_H

#include <stdio.h>
#include <stdlib.h>
#include "consts_n_structures.h"

double **randm(int size1, int size2);
int **mulmr(double coefficient, double **double_mat, int **target_mat, int size1, int size2, int oriented);
int **mat_create(int size1, int size2);

point_t *tri_graph_create(point_t *tar_graph, int graph_size);
void free_mat (size_t **target_mat, int size2);
void print_mat(int **target_mat, int size1, int size2);


#endif //INC_2_3_GRAPH_CREATE_H
