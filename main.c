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

// X routines
void init_x();
void close_x();
void redraw ();

//it`s not allowed to make height >= width / 1.15
int SCREEN_WIDTH = 1000;
int SCREEN_HEIGHT = 800;


// graph drawing routines
void arrow(double fi, int px, int py);
void draw_graph(point_t *graph, int size);

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

    rel_mat = mulmr(c, service_mat, rel_mat, n, n, 1);
    print_mat(rel_mat, n, n);

    point_t *graph;
    graph = tri_graph_create(graph, n);
    XEvent event;
    KeySym key;
    char text[255]; //keyboard buffer

    init_x();//window created

    while(1) {
        XNextEvent(dis, &event);
        switch (event.type) {
            case Expose:
                if (event.xexpose.count == 0) redraw();
                break;
            case KeyPress:
                if(XLookupString(&event.xkey, text, 255, &key, 0) == 1) {
                    if (text[0] == 'q') {
                        close_x();
                    }
                    else {
                        printf("You pressed the %c key!\n", text[0]);
                        draw_graph(graph, n);
                    }
                }
                break;
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

void arrow (double fi, int px, int py) {
    fi = 3.1416 * (180.0 - fi) / 180.0;
    int lx, ly, rx, ry;
    lx = px + (int) 15 * cos(fi + 0.3);
    rx = px + (int) 15 * cos(fi - 0.3);
    ly = py + (int) 15 * sin(fi + 0.3);
    ry = py + (int) 15 * sin(fi - 0.3);

    XDrawLine(dis, win, gc, lx, ly, px, py);
    XDrawLine(dis, win, gc, px, py, rx, ry);
}

point_t *tri_graph_create(point_t *tar_graph, int graph_size) {
    double as_ratio = (double) SCREEN_HEIGHT / SCREEN_WIDTH;
    tar_graph = malloc(sizeof (point_t) * graph_size);
    serv_point_t *serv_points = malloc(sizeof (serv_point_t) * graph_size);
    double indent_x = (double) 50 / SCREEN_WIDTH;
    double indent_y = (double) 50 / SCREEN_HEIGHT;
    int point_1 = 0;
    int point_2 = graph_size / 3;
    int point_3 = 2 * graph_size / 3;

    serv_points[point_1] = (serv_point_t) {0, 1 - indent_y};
    serv_points[point_2] = (serv_point_t) {1.15 - indent_x, -1 + indent_y};
    serv_points[point_3] = (serv_point_t) {-1.15 + indent_x, -1 + indent_y};

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
        tar_graph[i].x =  SCREEN_WIDTH - (int) (serv_points[i].x * SCREEN_WIDTH * as_ratio + SCREEN_WIDTH) / 2;
        tar_graph[i].y =  SCREEN_HEIGHT - (int) (serv_points[i].y * SCREEN_HEIGHT + SCREEN_HEIGHT) / 2 ;
        sprintf(tar_graph[i].number, "%d", i + 1);
        printf("%i %i %s\n", tar_graph[i].x, tar_graph[i].y, tar_graph[i].number);
    }
    free(serv_points);
    return tar_graph;
}

void draw_graph(point_t *graph, int size) {
    XSetForeground(dis, gc, 0xFFFFFF);
    int r = 15;
    for (int i = 0; i < size; i++) {
        XFillArc(dis, win, gc,
                 graph[i].x - r, graph[i].y - r,
                 2*r, 2*r,
                 0, 360 * 64);
    }
    unsigned long bpen = 0x3200FF;
    unsigned long kpen = 0xFFFFFF;

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
        XDrawString(dis, win, gc, graph[i].x - 2, graph[i].y + 4, graph[i].number, 2);
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
