#include "../graph_create.h"

double **randm(int size1, int size2) {
    double **result_mat = (double **) malloc(sizeof (double *) * size1);

    for (int i = 0; i < size1; i++) {
        result_mat[i] = (double *) malloc(sizeof(double ) * size2);

        for (int j = 0; j < size2; j++) {
            result_mat[i][j] = 2.0 * rand() / RAND_MAX;
        }
    }
    return result_mat;
}

int **mat_create(int size1, int size2) {
    int **result_mat = (int **) malloc(sizeof(int *) * size1);
    for (int i = 0; i < size1; i++) {
        result_mat[i] = (int *) malloc(sizeof(int) * size2);
    }
    return result_mat;
}

point_t *tri_graph_create(point_t *tar_graph, int graph_size) {
    double as_ratio = (double) SCREEN_HEIGHT / SCREEN_WIDTH;
    tar_graph = malloc(sizeof (point_t) * graph_size);
    serv_point_t *serv_points = malloc(sizeof (serv_point_t) * graph_size);
    double indent_x = (double) 100 / SCREEN_WIDTH;
    double indent_y = (double) 400 / SCREEN_HEIGHT;
    int point_1 = 0;
    int point_2 = graph_size / 3;
    int point_3 = 2 * graph_size / 3;

    serv_points[point_1] = (serv_point_t) {0, 1 - indent_y / 5};
    serv_points[point_2] = (serv_point_t) {1 - indent_x, -1 + indent_y};
    serv_points[point_3] = (serv_point_t) {-1 + indent_x, -1 + indent_y};

    //interpolation (I guess?) for non-anchor points of triangle
    for (int i = point_1 + 1, j = 1 ; i < point_2; i++, j++) {
        serv_points[i].x = serv_points[point_1].x +
                           j * (serv_points[point_2].x - serv_points[point_1].x) / (point_2 - point_1);
        serv_points[i].y = serv_points[point_1].y +
                           j * (serv_points[point_2].y - serv_points[point_1].y) / (point_2 - point_1);
    }
    for (int i = point_2 + 1, j = 1; i < point_3; i++, j++) {
        serv_points[i].x = serv_points[point_2].x +
                           j * (serv_points[point_3].x - serv_points[point_2].x) / (point_3 - point_2);
        serv_points[i].y = serv_points[point_2].y +
                           j * (serv_points[point_3].y - serv_points[point_2].y) / (point_3 - point_2);
    }
    for (int i = point_3 + 1, j = 1; i < graph_size; i++, j++) {
        serv_points[i].x = serv_points[point_3].x +
                           j * (serv_points[point_1].x - serv_points[point_3].x) / (graph_size - point_3);
        serv_points[i].y = serv_points[point_3].y +
                           j * (serv_points[point_1].y - serv_points[point_3].y) / (graph_size - point_3);
    }
    //translating into the screen space
    for (int i = 0; i < graph_size; i++) {
        tar_graph[i].x =  SCREEN_WIDTH + (int) (serv_points[i].x * SCREEN_WIDTH * as_ratio - SCREEN_WIDTH) / 2;
        tar_graph[i].y =  SCREEN_HEIGHT - (int) (serv_points[i].y * SCREEN_HEIGHT + SCREEN_HEIGHT) / 2 ;
        sprintf(tar_graph[i].number, "%d", i + 1);
    }
    free(serv_points);
    return tar_graph;
}

int **mulmr(double coefficient, double **double_mat, int **target_mat, int size1, int size2, int oriented) {
    for (int i = 0; i < size1; i++) {
        for (int j = 0; j < size2; j++) {
            if (!oriented) {
                target_mat[i][j] = coefficient * double_mat[i][j] < 1 ?
                                   0 : 1;
                if (target_mat[i][j]) target_mat[j][i] = 1;
            }
            else {
                target_mat[i][j] = coefficient * double_mat[i][j] < 1 ?
                                   0 : 1;
            }
        }
    }
    return target_mat;
}

void free_mat (size_t **target_mat, int size2) {
    for (int i = 0; i <= size2; i++) {
        free(target_mat[i]);
    }
    free(target_mat);
}

void print_mat(int **target_mat, int size1, int size2) {
    for (int i = 0; i < size1; i++) {
        for (int j = 0; j < size2; j++) {
            printf("%2i ", target_mat[i][j]);
        }
        printf("\n");
    }
};
