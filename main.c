// X library
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

// other libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "src/consts_n_structures.h"
#include "src/graph_create.h"
#include "src/vec_math_lib.h"
#include "src/X_routines.h"

// graph drawing routines
void arrow(double theta, int px, int py);
void draw_graph_vertices (point_t *graph, int size);

//за цю функію взагалі треба 13 балів ставити)
void draw_arc (vec_4 *vector, int offset_radius, int with_arrow) {
    vec_2 *vec = malloc(sizeof (vec_2));
    vec_2 *vec_OX = malloc(sizeof (vec_2));
    *vec_OX = (vec_2) {1,  0};

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

    vec_2 * center_to_end = malloc(sizeof (vec_2));
    center_to_end->x = vector->end_x - center_x;
    center_to_end->y = vector->end_y - center_y;

    vec_2 * center_to_start = malloc(sizeof (vec_2));
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
    if(with_arrow) {
        int theta = -90 + round(acos(vectors_cos(center_to_end, vec_OX)) * 180 / 3.1415);
        if (vec_quarter(center_to_end) == 3 || vec_quarter(center_to_end) == 4)
            theta = 180 - theta;
        arrow(theta, vector->end_x, vector->end_y);
    }
    free(vec); free(vec_OX); free(center_to_end); free(center_to_start);
}
void draw_line (vec_4 *vector, int with_arrow) {
    XSetLineAttributes(dis, gc,
                       1,
                       LineSolid,
                       CapButt, JoinMiter);
    XDrawLine(dis, win, gc, vector->start_x, vector->start_y, vector->end_x, vector->end_y);

    if (with_arrow) {
        vec_2 *vec = malloc(sizeof (vec_2));
        vec_2 *vec_OX = malloc(sizeof (vec_2));
        *vec_OX = (vec_2) {1, 0};
        vec->x = vector->end_x - vector->start_x;
        vec->y = vector->end_y - vector->start_y;
        double cos = vectors_cos(vec, vec_OX);
        int theta = acos(cos) * 180 / 3.1415;
        if (vec_quarter(vec) == 3 || vec_quarter(vec) == 4)
            theta = 360 - theta;
        arrow(theta, vector->end_x, vector->end_y);
        free (vec); free (vec_OX);
    }
}
void draw_graph_u (point_t *graph, int **relation_matrix, int size) {
    int point_1 = 0;
    int point_2 = size / 3;
    int point_3 = 2 * size / 3;
    for (int i = 0; i < size; i++) {
        for (int j = i; j < size; j++) {
            XSetForeground(dis, gc, 0xFFFFFF);
            XSetLineAttributes(dis, gc, 1, LineSolid, CapButt, JoinMiter);

            vec_4 *this_vec = malloc(sizeof (vec_4));
            *this_vec = (vec_4) {graph[i].x, graph[i].y,
                                 graph[j].x, graph[j].y};

            r_offset *offset = rad_offsetting(this_vec);

            this_vec->start_x += offset->x_offset;
            this_vec->start_y += offset->y_offset;
            this_vec->end_x -= offset->x_offset;
            this_vec->end_y -= offset->y_offset;

            int condition = (
                    point_1 <= i && i <= point_2 && point_1 <= j && j <= point_2 ||
                    point_2 <= i && i <= point_3 && point_2 <= j && j <= point_3 ||
                    point_3 <= i && i < size && point_3 <= j && j < size ||
                    point_3 <= i && i < size &&  j == point_1 ||
                    point_3 <= j && j < size &&  i == point_1);
            if (condition) {
                switch ((abs(i - j) % (size - 1))) {
                    case 0:
                        if (relation_matrix[i][i]) {

                            XDrawArc(dis, win, gc, this_vec->start_x + 0.3*POINT_RADIUS, this_vec->start_y,
                                     2 * POINT_RADIUS, 2 * POINT_RADIUS, 240*64, 240*64);
                        }
                        break;
                    case 1:
                        if (relation_matrix[i][j]) {
                            draw_line(this_vec, 0);
                        }
                        break;
                    default:
                        if (relation_matrix[i][j]) {
                            draw_arc (this_vec, 500, 0);
                        }
                }
            }
            else {
                if (relation_matrix[i][j]) {
                    draw_line(this_vec, 0);
                }
            }
            free (this_vec); free (offset);
        }
    }
}

void draw_graph_o (point_t *graph, int **relation_matrix, int size) {

    int point_1 = 0;
    int point_2 = size / 3;
    int point_3 = 2 * size / 3;


    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            XSetForeground(dis, gc, 0xFFFFFF);
            XSetLineAttributes(dis, gc, 1, LineSolid, CapButt, JoinMiter);

            vec_4 *this_vec = malloc(sizeof (vec_4));
            *this_vec = (vec_4) {graph[i].x, graph[i].y,
                                 graph[j].x, graph[j].y};

            r_offset *offset = rad_offsetting(this_vec);

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

                    if (condition) {
                        switch (abs(i - j) % (size - 1)) {
                            case 0:
                                if (relation_matrix[i][i]) {
                                    XDrawArc(dis, win, gc, this_vec->start_x + 0.3*POINT_RADIUS, this_vec->start_y,
                                             2 * POINT_RADIUS, 2 * POINT_RADIUS, 240*64, 240*64);
                                    arrow(-190, this_vec->start_x + 0.7*POINT_RADIUS, this_vec->start_y + 1.9*POINT_RADIUS);
                                }
                                break;
                            case 1:
                                if (relation_matrix[i][j]) {
                                    if (relation_matrix[j][i])
                                        draw_arc (this_vec, 500, 1);
                                    else
                                        draw_line(this_vec, 1);
                                }
                                break;
                            default:
                                if (relation_matrix[i][j]) {
                                    draw_arc (this_vec, 500, 1);
                                }
                        }
                    }
                    else {
                        if (relation_matrix[i][j]) {
                            if (relation_matrix[j][i])
                                draw_arc (this_vec, 500, 1);
                            else
                                draw_line(this_vec, 1);
                        }
                }
            }
            free (this_vec); free(offset);
        }
    }
}
// matrix functions


int main() {
    int n1 = 2, n2 = 1, n3 = 2, n4 = 5;
    srand(n1 * 1000 + n2 * 100 + n3 * 10 + n4);

    int n = 10 + n3;
    double c = 1.0 - n3 * 0.02 - n4 * 0.005 - 0.25;
    double **service_mat;
    service_mat = randm(n, n);
    int **rel_mat;
    rel_mat = mat_create(n, n);
    point_t *graph;

    XEvent event;
    KeySym key;
    char text[255]; //keyboard buffer

    init_x();//window created

    while (1) {
        XNextEvent(dis, &event);
        switch (event.type) {
            case Expose:
                if (event.xexpose.count == 0) redraw();
                break;
            case KeyPress:
                if (XLookupString(&event.xkey, text, 255, &key, 0) == 1) {
                    switch (text[0]) {
                        case 'q':
                            free_mat(service_mat, n);
                            free_mat(rel_mat, n);
                            free(graph);
                            close_x();
                            return 0;
                            break;
                        case 'o':
                            redraw();
                            rel_mat = mulmr(c, service_mat, rel_mat, n, n, 1);
                            printf("drawing oriented relation matrix: \n");
                            print_mat(rel_mat, n, n);
                            graph = tri_graph_create(graph, n);
                            draw_graph_o(graph, rel_mat, n);
                            draw_graph_vertices(graph, n);
                            break;
                        case 'u':
                            redraw();
                            rel_mat = mulmr(c, service_mat, rel_mat, n, n, 0);
                            printf("drawing unoriented relation matrix: \n");
                            print_mat(rel_mat, n, n);
                            graph = tri_graph_create(graph, n);
                            draw_graph_u(graph, rel_mat, n);
                            draw_graph_vertices(graph, n);
                            break;

                    }
                    break;
                }
        }
    }
}


void init_x() {
    unsigned long background = 0x3D3D3D;

    dis = XOpenDisplay((char *)0);
    screen = DefaultScreen(dis);

    win = XCreateSimpleWindow(dis, DefaultRootWindow(dis), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 0x000000, background);

    XSetStandardProperties(dis, win, ProgramName, ProgramName, None, NULL, 0, NULL);
    XSelectInput(dis, win, ExposureMask | KeyPressMask);

    gc = XCreateGC(dis, win, 0, 0);
    XMapRaised(dis, win);

}

void close_x() {
    XFreeGC(dis, gc);
    XDestroyWindow(dis, win);
    XCloseDisplay(dis);
    exit(0);
}

void redraw() {
    XClearWindow(dis, win);

}

void arrow (double theta, int px, int py) {
    XSetForeground(dis, gc, 0xFF0000);
    XSetLineAttributes(dis, gc,
                       2,
                       LineSolid,
                       CapButt, JoinMiter);
    theta = 3.1415 * (180.0 - theta) / 180.0;
    int lx, ly, rx, ry;
    lx = px + (int) 20 * cos(theta + 0.3);
    rx = px + (int) 20 * cos(theta - 0.3);
    ly = py + (int) 20 * sin(theta + 0.3);
    ry = py + (int) 20 * sin(theta - 0.3);

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