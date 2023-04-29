#include "../graph_draw.h"
#include <stdio.h>

a_list_t *arrows_list = NULL;

void arrow_add (a_list_t **tar_p, double theta, int px, int py) {
    a_list_t *node_p;
    node_p = malloc(sizeof (a_list_t));
    node_p->theta = theta;
    node_p->px = px;
    node_p->py = py;
    if (*tar_p) {
        node_p->next_arr = *tar_p;
    }
    else {
        node_p->next_arr = NULL;
    }
    *tar_p = node_p;
}

void arrows_clear (a_list_t **tar_p) {
    a_list_t *tar = *tar_p;
    if (!tar) return;
    a_list_t *next_p;
    while (tar) {
        next_p = tar->next_arr;
        free (tar);
        tar = next_p;
    }
    *tar_p = NULL;
}

void arrows_draw (a_list_t *tar) {
    a_list_t *next_p;
    while (tar) {
        next_p = tar->next_arr;
        arrow (tar->theta, tar->px, tar->py);
        tar = next_p;
    }
}

void arrow (double theta, int px, int py) {
    theta = 3.1415 * (180.0 - theta) / 180.0;
    int lx, ly, rx, ry;
    lx = px + (int) 20 * cos(theta + 0.3);
    rx = px + (int) 20 * cos(theta - 0.3);
    ly = py + (int) 20 * sin(theta + 0.3);
    ry = py + (int) 20 * sin(theta - 0.3);

    XSetForeground(dis, gc, 0x212A3E);
    XSetLineAttributes(dis, gc,
                       4,
                       LineSolid,
                       CapButt, JoinMiter);
    XDrawLine(dis, win, gc, lx, ly, px, py);
    XDrawLine(dis, win, gc, px, py, rx, ry);

    XSetForeground(dis, gc, 0xFC4F00);
    XSetLineAttributes(dis, gc,
                       2,
                       LineSolid,
                       CapButt, JoinMiter);
    XDrawLine(dis, win, gc, lx, ly, px, py);
    XDrawLine(dis, win, gc, px, py, rx, ry);
}

void draw_graph_vertices(point_t *graph, int size) {
    XSetForeground(dis, gc, 0xFFFFFF);
    for (int i = 0; i < size; i++) {
        XFillArc(dis, win, gc,
                 graph[i].x - POINT_RADIUS, graph[i].y - POINT_RADIUS,
                 2 * POINT_RADIUS, 2 * POINT_RADIUS,
                 0, 360 * 64);
    }
    XSetForeground(dis, gc, bpen);

    XSetLineAttributes(dis, gc,
                       2,
                       LineSolid,
                       CapButt, JoinMiter);
    for (int i = 0; i < size; i++){
        XDrawArc(dis, win, gc, graph[i].x - POINT_RADIUS, graph[i].y - POINT_RADIUS,
                 2 * POINT_RADIUS, 2 * POINT_RADIUS, 0, 360 * 64);
        XDrawString(dis, win, gc, graph[i].x - 2, graph[i].y + 4,
                    graph[i].number, i < 9 ? 1 : 2);
    }
}

//за цю функію взагалі треба 13 балів ставити)
void draw_arc(vec_4_t *vector, int offset_radius) {
    vec_2_t *vec = malloc(sizeof (vec_2_t));
    vec_2_t *vec_OX = malloc(sizeof (vec_2_t));
    *vec_OX = (vec_2_t) {1, 0};

    vec->x = vector->end_x - vector->start_x;
    vec->y = vector->end_y - vector->start_y;

    int quarter = vec_quarter(vec);

    int midpoint_x = vec->x / 2;
    int midpoint_y = vec->y / 2;
    double cosB = vectors_cos(vec, vec_OX);
    double sinA = cosB;
    double cosA;
    if (quarter == 1 || quarter == 2) cosA = -sqrt(1-cosB * cosB);
    else cosA = sqrt(1-cosB * cosB);

    int offset_y = sinA * offset_radius;
    int offset_x = cosA * offset_radius;
    int center_x = vector->start_x + midpoint_x - offset_x;
    int center_y = vector->start_y + midpoint_y + offset_y;
    int rad2 = sqrt((center_x - vector->start_x) * (center_x - vector->start_x) +
                    (center_y - vector->start_y) * (center_y - vector->start_y));

    vec_2_t * center_to_end = malloc(sizeof (vec_2_t));
    center_to_end->x = vector->end_x - center_x;
    center_to_end->y = vector->end_y - center_y;

    vec_2_t * center_to_start = malloc(sizeof (vec_2_t));
    center_to_start->x = vector->start_x - center_x;
    center_to_start->y = vector->start_y - center_y;

    cosA = vectors_cos(center_to_start, vec_OX);
    int S = 64 * acos(cosA) * 180 / 3.1415;
    if (vec_quarter(center_to_start) == 4 || vec_quarter(center_to_start) == 3)
        S = 360 * 64 - S;
    cosB = vectors_cos(center_to_end, vec_OX);
    int E = 64 * acos(cosB) * 180 / 3.1415;

    if (vec_quarter(center_to_end) == 4 || vec_quarter(center_to_end) == 3)
        E = 360 * 64 - E;

    if (vec_quarter(center_to_end) == 4 && vec_quarter(center_to_start) == 1)
        S = 360 * 64 + S;

    XSetLineAttributes(dis, gc,
                       1,
                       LineSolid,
                       CapButt, JoinMiter);

    XDrawArc(dis, win, gc, center_x - rad2, center_y - rad2,
             2*rad2, 2*rad2, E, abs(S - E));

    int theta = -90 + round(acos(vectors_cos(center_to_end, vec_OX)) * 180 / 3.1415);
    if (vec_quarter(center_to_end) == 3 || vec_quarter(center_to_end) == 4)
        theta = 180 - theta;
    arrow_add(&arrows_list, theta, vector->end_x, vector->end_y);
    free(vec); free(vec_OX); free(center_to_end); free(center_to_start);
}

void draw_line(vec_4_t *vector) {
    XSetLineAttributes(dis, gc,
                       1,
                       LineSolid,
                       CapButt, JoinMiter);
    XDrawLine(dis, win, gc, vector->start_x, vector->start_y, vector->end_x, vector->end_y);

    vec_2_t *vec = malloc(sizeof (vec_2_t));
    vec_2_t *vec_OX = malloc(sizeof (vec_2_t));
    *vec_OX = (vec_2_t) {1, 0};
    vec->x = vector->end_x - vector->start_x;
    vec->y = vector->end_y - vector->start_y;
    double cos = vectors_cos(vec, vec_OX);
    int theta = acos(cos) * 180 / 3.1415;
    if (vec_quarter(vec) == 3 || vec_quarter(vec) == 4)
        theta = 360 - theta;
    arrow_add(&arrows_list, theta, vector->end_x, vector->end_y);

    free (vec); free (vec_OX);
}

void draw_graph(point_t *graph, int **relation_matrix, int size, int oriented) {

    int point_1 = 0;
    int point_2 = size / 3;
    int point_3 = 2 * size / 3;

    for (int i = 0; i < size; i++) {
        for (int j = oriented ? 0 : i; j < size; j++) {
            XSetForeground(dis, gc, 0xFFFFFF);
            XSetLineAttributes(dis, gc, 1, LineSolid, CapButt, JoinMiter);

            vec_4_t *this_vec = malloc(sizeof (vec_4_t));
            *this_vec = (vec_4_t) {graph[i].x, graph[i].y,
                                   graph[j].x, graph[j].y};

            r_offset_t *offset = rad_offsetting(this_vec);

            this_vec->start_x += offset->x_offset;
            this_vec->start_y += offset->y_offset;
            this_vec->end_x -= offset->x_offset;
            this_vec->end_y -= offset->y_offset;

            if (relation_matrix[i][j]) {
                int condition = (
                        point_1 <= i && i <= point_2 && point_1 <= j && j <= point_2 ||
                        point_2 <= i && i <= point_3 && point_2 <= j && j <= point_3 ||
                        point_3 <= i && i < size && point_3 <= j && j < size ||
                        point_3 <= i && i < size &&  j == point_1 ||
                        point_3 <= j && j < size &&  i == point_1);

                if (condition) { //на одній лінії знаходяться
                    switch (abs(i - j) % (size - 1)) {
                        case 0:
                            if (relation_matrix[i][i]) {
                                XDrawArc(dis, win, gc, this_vec->start_x + 0.3*POINT_RADIUS, this_vec->start_y,
                                         2 * POINT_RADIUS, 2 * POINT_RADIUS, 240*64, 240*64);
                                if (oriented) arrow_add(&arrows_list,
                                                        -190, this_vec->start_x + 0.7*POINT_RADIUS, this_vec->start_y + 1.9*POINT_RADIUS);
                            }
                            break;
                        case 1:
                            if (relation_matrix[i][j]) {
                                if (relation_matrix[j][i] && oriented)
                                    draw_arc(this_vec, 500);
                                else
                                    draw_line(this_vec);
                            }
                            break;
                        default:
                            if (relation_matrix[i][j])
                                draw_arc(this_vec, 500);
                    }
                }
                else { //на різних лініях
                    if (relation_matrix[i][j]) {
                        if (oriented) {
                            if (relation_matrix[j][i])
                                draw_arc(this_vec, 500);
                            else
                                draw_line(this_vec);
                        }
                        else if (relation_matrix[i][j])
                            draw_line(this_vec);
                    }
                }
            }
            free (this_vec); free(offset);
        }
    }
    if (oriented) arrows_draw(arrows_list);
    arrows_clear(&arrows_list);
}