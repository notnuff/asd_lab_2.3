// X library
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

// other libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// X variables
Display *dis;
int screen;
Window win;
GC gc; //graphic context

// Program name
char ProgramName[] = "lab3";

//virtual screen space has double values from -1 to 1
typedef struct virtual_screen_space_point_t {
    double x;
    double y;
} serv_point_t;
//actual screen space has int values from 0 to screen width (height)
typedef struct screen_space_point_t {
    int x;
    int y;
    char number[2];
} point_t;

typedef struct vector {
    int start_x;
    int start_y;
    int end_x;
    int end_y;
} vec_4;


typedef struct serv_vector { //it`s relative to start_x, start_y
    int x;
    int y;
} vec_2;

// X routines
void init_x();
void close_x();
void redraw ();


int SCREEN_WIDTH = 1000;
int SCREEN_HEIGHT = 800;
unsigned long bpen = 0x3200FF;
unsigned long wpen = 0xFFFFFF;



// graph drawing routines
void arrow(double theta, int px, int py);
double vec_dist (vec_2* vec) {
    double dist = sqrt(vec->x * vec->x + vec->y * vec->y);
    return dist;
}
double vectors_cos (vec_2* vec1, vec_2* vec2) {
    double cos = (vec1->x * vec2 ->x + vec1->y * vec2->y) /
            vec_dist(vec1) * vec_dist(vec2);
    return cos;
}
void draw_graph_vertices (point_t *graph, int size);

//за цю функію взагалі треба 13 балів ставити)
void draw_arc (vec_4 *vector, int radius) {
    vec_2 *vec = malloc(sizeof (vec_2));
    vec_2 *vec_OX = malloc(sizeof (vec_2));
    *vec_OX = (vec_2) {1,  0};
    vec->x = vector->end_x - vector->start_x;
    vec->y = vector->end_y - vector->start_y;
    int midpoint_x = vec->x / 2;
    int midpoint_y = vec->y / 2;
    double cosB = vectors_cos(vec, vec_OX);
    double sinA = cosB;
    double cosA = cosB > 0 ? sqrt(1-cosB * cosB) : -sqrt(1-cosB * cosB);
    int offset_y = sinA * radius;
    int offset_x = cosA * radius;
    int center_x = vector->start_x + midpoint_x - offset_x;
    int center_y = vector->start_y + midpoint_y + offset_y;
    int rad2 = sqrt((center_x - vector->start_x) * (center_x - vector->start_x) +
            (center_y - vector->start_y) * (center_y - vector->start_y));
    vec->x = -(center_x - vector->end_x);
    vec->y = -(center_y - vector->end_y);
    cosB = vectors_cos(vec, vec_OX);
    vec->x = -(center_x - vector->start_x);
    vec->y = -(center_y - vector->start_y);
    cosA = vectors_cos(vec, vec_OX);

    int B = 64 * acos(cosB) * 180 / 3.1415;
    if (cosB > 0) B = -B;
    int A = 64 * acos(cosA) * 180 / 3.1415;
    if (cosA > 0) A = -A;
    printf("\n %i %i \n", A/64, B/64);
    XDrawLine(dis, win, gc, center_x, center_y, vector->start_x + midpoint_x, vector->start_y + midpoint_y);
    XDrawArc(dis, win, gc, center_x - rad2, center_y - rad2,
             2*rad2, 2*rad2, B, abs(B - A));
}
void draw_graph_o (point_t *graph, int **relation_matrix, int size) {
    int point_1 = 0;
    int point_2 = size / 3;
    int point_3 = 2 * size / 3;
    XSetForeground(dis, gc, wpen);
    for (int i = point_1; i < point_2; i++) {
        for (int j = 0; j < size; j++) {
            int x_offset = 2 * (graph[j].x - graph[i].x);
            int y_offset = 2 * (graph[j].y - graph[i].y);

            if (j < point_2) {
                switch (abs(i - j)) {
                    case 0:
                        if (relation_matrix[i][i]) {
                            XDrawArc(dis, win, gc, graph[j].x + 5, graph[j].y - 20,
                                     2 * 15, 2 * 15, 0, 360 * 64);
                        }
                        break;
                    case 1:
                        if (relation_matrix[i][j]) {
                            XDrawLine(dis, win, gc, graph[i].x, graph[i].y, graph[j].x, graph[j].y);
                        }
                        break;
                    default:
                        if (relation_matrix[i][j]) {
                            vec_4 *this_vec = malloc(sizeof (vec_4));
                            *this_vec = (vec_4) {graph[i].x, graph[i].y,
                                                 graph[j].x, graph[j].y};

                            draw_arc (this_vec, 200);
                        }
                }
            }
            else {
                if (relation_matrix[i][j]) {
                    XDrawLine(dis, win, gc, graph[i].x, graph[i].y, graph[j].x, graph[j].y);
                }
            }
        }
    }
}

// matrix functions
double **randm(int size1, int size2);
int **mulmr(double coefficient, double **double_mat, int **target_mat, int size1, int size2, int oriented);
int **mat_create(int size1, int size2);

point_t *tri_graph_create(point_t *tar_graph, int graph_size);
void free_mat (size_t **target_mat, int size2);
void print_mat(int **target_mat, int size1, int size2);


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
                            printf("drawing oriented matrix: \n");
                            print_mat(rel_mat, n, n);
                            graph = tri_graph_create(graph, n);
                            draw_graph_o(graph, rel_mat, n);
                            draw_graph_vertices(graph, n);

                            break;
                        case 'u':

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
    theta = 3.1416 * (180.0 - theta) / 180.0;
    int lx, ly, rx, ry;
    lx = px + (int) 15 * cos(theta + 0.3);
    rx = px + (int) 15 * cos(theta - 0.3);
    ly = py + (int) 15 * sin(theta + 0.3);
    ry = py + (int) 15 * sin(theta - 0.3);

    XDrawLine(dis, win, gc, lx, ly, px, py);
    XDrawLine(dis, win, gc, px, py, rx, ry);
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
        printf("%i %i %s\n", tar_graph[i].x, tar_graph[i].y, tar_graph[i].number);
    }
    free(serv_points);
    return tar_graph;
}

void draw_graph_vertices(point_t *graph, int size) {
    XSetForeground(dis, gc, 0xFFFFFF);
    int r = 15;
    for (int i = 0; i < size; i++) {
        XFillArc(dis, win, gc,
                 graph[i].x - r, graph[i].y - r,
                 2*r, 2*r,
                 0, 360 * 64);
    }

//    XSetForeground(dis, gc, kpen);
//    XDrawLine(dis, win, gc, nx[0], ny[0], nx[1], ny[1]);
//    arrow(0, nx[1] - dx, ny[1]);
//
//    XDrawArc(dis, win, gc, nx[0], ny[0] - 40, nx[2] - nx[0], 80, 0, 180*64);
//    arrow(-45.0, nx[2] - dx * sin(45), ny[2] - dy * sin(45));

    XSetForeground(dis, gc, bpen);

    XSetLineAttributes(dis, gc,
                       2,
                       LineSolid,
                       CapButt, JoinMiter);
    for (int i = 0; i < size; i++){
        XDrawArc(dis, win, gc, graph[i].x - r, graph[i].y - r,
                 2*r, 2*r, 0, 360*64);
        XDrawString(dis, win, gc, graph[i].x - 2, graph[i].y + 4,
                    graph[i].number, i < 9 ? 1 : 2);
    }
}



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

int **mulmr(double coefficient, double **double_mat, int **target_mat, int size1, int size2, int oriented) {
    for (int i = 0; i < size1; i++) {
        for (int j = 0; j < size2; j++) {
            if (oriented) {
                target_mat[i][j] = coefficient * double_mat[i][j] < 1 ?
                                   0 : 1;
                target_mat[j][i] = coefficient * double_mat[i][j] < 1 ?
                                   0 : 1;
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
