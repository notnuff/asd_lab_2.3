#ifndef INC_2_3_VEC_MATH_LIB_H
#define INC_2_3_VEC_MATH_LIB_H

#include <math.h>
#include <stdlib.h>
#include "consts_n_structures.h"

double vec_dist (vec_2_t* vec);
double vectors_cos (vec_2_t* vec1, vec_2_t* vec2);
int vec_quarter (vec_2_t* vec);
r_offset_t *rad_offsetting(vec_4_t* vector);
#endif //INC_2_3_VEC_MATH_LIB_H
