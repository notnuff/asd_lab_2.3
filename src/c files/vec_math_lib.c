#include "../vec_math_lib.h"

double vec_dist (vec_2_t* vec) {
    double dist = sqrt(vec->x * vec->x + vec->y * vec->y);
    return dist;
}
double vectors_cos (vec_2_t* vec1, vec_2_t* vec2) {
    double cos = (vec1->x * vec2 ->x + vec1->y * vec2->y) /
                 (vec_dist(vec1) * vec_dist(vec2));
    return (cos > 0.01 || cos < -0.01) ? cos : 0;
}
int vec_quarter (vec_2_t* vec) {
    if (vec->x > 0 && vec->y > 0) return 4;
    if (vec->x > 0 && vec->y <= 0) return 1;
    if (vec->x <= 0 && vec->y > 0) return 3;
    if (vec->x <= 0 && vec->y <= 0) return 2;
}
r_offset_t *rad_offsetting(vec_4_t* vector) {
    vec_2_t *vec = malloc(sizeof (vec_2_t));
    vec_2_t *vec_OX = malloc(sizeof (vec_2_t));

    *vec_OX = (vec_2_t) {1, 0};
    vec->x = vector->end_x - vector->start_x;
    vec->y = vector->end_y - vector->start_y;

    int quarter = vec_quarter(vec);

    int x = abs(POINT_RADIUS * vectors_cos(vec, vec_OX));
    int y = abs(POINT_RADIUS * sqrt(1 - vectors_cos(vec, vec_OX) * vectors_cos(vec, vec_OX)));

    switch (quarter) {
        case 1:
            x = x;
            y = -y;
            break;
        case 2:
            x = -x;
            y = -y;
            break;
        case 3:
            x = -x;
            y = y;
            break;
        case 4:
            x = x;
            y = y;
            break;
    }
    free(vec_OX); free(vec);
    r_offset_t *offset = malloc(sizeof(r_offset_t));
    offset->x_offset = x;
    offset->y_offset = y;
    return offset;
}