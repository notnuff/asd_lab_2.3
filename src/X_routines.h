//
// Created by notnuff on 25.04.23.
//

#ifndef INC_2_3_X_ROUTINES_H
#define INC_2_3_X_ROUTINES_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include "consts_n_structures.h"

Display *dis;
int screen;
Window win;
GC gc; //graphic context
char ProgramName[] = "lab3";

void init_x();
void close_x();
void redraw ();

#endif //INC_2_3_X_ROUTINES_H
