#include "../vec_math_lib.h"

double vec_dist (vec_2* vec) {
    double dist = sqrt(vec->x * vec->x + vec->y * vec->y);
    return dist;
}
double vectors_cos (vec_2* vec1, vec_2* vec2) {
    double cos = (vec1->x * vec2 ->x + vec1->y * vec2->y) /
                 (vec_dist(vec1) * vec_dist(vec2));
    return (cos > 0.01 || cos < -0.01) ? cos : 0;
}
int vec_quarter (vec_2* vec) {
    if (vec->x > 0 && vec->y > 0) return 4;
    if (vec->x > 0 && vec->y <= 0) return 1;
    if (vec->x <= 0 && vec->y > 0) return 3;
    if (vec->x <= 0 && vec->y <= 0) return 2;
}
r_offset *rad_offsetting(vec_4* vector) {
    vec_2 *vec = malloc(sizeof (vec_2));
    vec_2 *vec_OX = malloc(sizeof (vec_2));

    *vec_OX = (vec_2) {1,  0};
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
    r_offset *offset = malloc(sizeof(r_offset));
    offset->x_offset = x;
    offset->y_offset = y;
    return offset;
}