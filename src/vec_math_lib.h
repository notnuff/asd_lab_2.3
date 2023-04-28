#ifndef INC_2_3_VEC_MATH_LIB_H
#define INC_2_3_VEC_MATH_LIB_H

#include <math.h>
#include <stdlib.h>
#include "consts_n_structures.h"

double vec_dist (vec_2* vec);
double vectors_cos (vec_2* vec1, vec_2* vec2);
int vec_quarter (vec_2* vec);
r_offset *rad_offsetting(vec_4* vector);
#endif //INC_2_3_VEC_MATH_LIB_H
