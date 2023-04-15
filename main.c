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

// X routines
void init_x();
void close_x();
void redraw ();

// graph drawing routines
void arrow(double fi, int px, int py);
void draw_graph();

// matrix functions
double **randm(int size1, int size2);
int **mulmr(double coefficient, double **double_mat, int **target_mat, int size1, int size2, int oriented);
int **mat_create(int size1, int size2);
void print_mat(int **target_mat, int size1, int size2);

int main() {
    int n1 = 2, n2 = 1, n3 = 2, n4 = 5;
    srand(n1 * 1000 + n2 * 100 + n3 * 10 + n4);

    int n = 10 + n3;
    double c = 1.0 - n3 * 0.02 - n4 * 0.005 - 0.25;
    double **service_mat;
    service_mat = randm(n, n);
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            printf("%5.4f ", service_mat[i][j] * c);
        }
        printf("\n");
    }

    int **rel_mat;
    rel_mat = mat_create(n, n);

    rel_mat = mulmr(c, service_mat, rel_mat, n, n, 0);
    print_mat(rel_mat, n, n);

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
                        draw_graph();
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

    win = XCreateSimpleWindow(dis, DefaultRootWindow(dis), 0, 0, 640, 360, 1, 0x000000, background);

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
    lx = px + 15 * cos(fi + 0.3);
    rx = px + 15 * cos(fi - 0.3);
    ly = py + 15 * sin(fi + 0.3);
    ry = py + 15 * sin(fi - 0.3);

    XDrawLine(dis, win, gc, lx, ly, px, py);
    XDrawLine(dis, win, gc, px, py, rx, ry);
}

void draw_graph() {
    char *nn[3] = {"1", "2", "3"};
    int nx[3] = {100, 200, 300};
    int ny[3] = {100, 100, 100};
    int dx = 16, dy = 16, dtx = 5;
    int i;

    unsigned long bpen = 0x3200FF;
    unsigned long kpen = 0xFFFFFF;

    XSetForeground(dis, gc, kpen);
    XDrawLine(dis, win, gc, nx[0], ny[0], nx[1], ny[1]);
    arrow(0, nx[1] - dx, ny[1]);

    XDrawArc(dis, win, gc, nx[0], ny[0] - 40, nx[2] - nx[0], 80, 0, 180*64);
    arrow(-45.0, nx[2] - dx * sin(45), ny[2] - dy * sin(45));

    XSetForeground(dis, gc, 0xFFFFFF);

    for (i = 0; i <= 2; i++) {
        XFillArc(dis, win, gc,
                 nx[i] - dx, ny[i] - dy,
                 2*dx, 2*dy,
                 0, 360 * 64);
    }
    XSetForeground(dis, gc, bpen);

    XSetLineAttributes(dis, gc,
                       2,
                       LineSolid,
                       CapButt, JoinMiter);
    for (i = 0; i <= 2; i++){
        XDrawArc(dis, win, gc, nx[i] - dx, ny[i] - dy,
                 2*dx, 2*dy, 0, 360*64);
        XDrawString(dis, win, gc, nx[i], ny[i], nn[i], 1);
    }
}

double **randm(int size1, int size2) {
    double **result_mat = (double **) malloc(sizeof (double *) * size1);

    for (int i = 0; i <= size1; i++) {
        result_mat[i] = (double *) malloc(sizeof(double ) * size2);

        for (int j = 0; j <= size2; j++) {
            result_mat[i][j] = 2.0 * rand() / RAND_MAX;
        }
    }
    return result_mat;
}

int **mat_create(int size1, int size2) {
    int **result_mat = (int **) malloc(sizeof(int *) * size1);
    for (int i = 0; i <= size1; i++) {
        result_mat[i] = (int *) malloc(sizeof(int) * size2);
    }
    return result_mat;
}

int **mulmr(double coefficient, double **double_mat, int **target_mat, int size1, int size2, int oriented) {
    for (int i = 0; i <= size1; i++) {
        for (int j = 0; j <= size2; j++) {
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

void print_mat(int **target_mat, int size1, int size2) {
    for (int i = 0; i <= size1; i++) {
        for (int j = 0; j <= size2; j++) {
            printf("%2i ", target_mat[i][j]);
        }
        printf("\n");
    }
};
