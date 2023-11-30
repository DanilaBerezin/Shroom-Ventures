#ifndef DEBUG_H
#define DEBUG_G

// WARNING: include this header after ALL header files since it will redefine symbols

#ifdef DEBUG
#include <stdio.h>
#endif

// Debugging macros
#ifdef DEBUG
    #define PRINT_FPS(x,y) DrawFPS((x), (y))
#else 
    #define PRINT_FPS(x,y)
#endif

#endif
