// other libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "src/consts_n_structures.h"
#include "src/graph_create.h"
#include "src/vec_math_lib.h"
#include "src/X_routines.h"
#include "src/graph_draw.h"


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
                if (event.xexpose.count == 0) redraw_x();
                break;
            case KeyPress:
                if (XLookupString(&event.xkey, text, 255, &key, 0) == 1) {
                    int oriented = 42;
                    switch (text[0]) {
                        case 'q':
                            free_mat(service_mat, n);
                            free_mat(rel_mat, n);
                            free(graph);
                            close_x();
                            return 0;
                            break;
                        case 'o':
                            oriented = 1;
                            printf("drawing oriented relation matrix: \n");
                            break;
                        case 'u':
                            oriented = 0;
                            printf("drawing unoriented relation matrix: \n");
                            break;
                        default:
                            redraw_x();
                            oriented = 42;
                    }
                    if (oriented != 42) {
                        redraw_x();
                        rel_mat = mulmr(c, service_mat, rel_mat, n, n, oriented);
                        print_mat(rel_mat, n, n);
                        graph = tri_graph_create(graph, n);
                        draw_graph(graph, rel_mat, n, oriented);
                        draw_graph_vertices(graph, n);
                    }
                    break;
                }
        }
    }
}
