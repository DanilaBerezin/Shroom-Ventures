#ifndef DEBUG_H
#define DEBUG_G

// WARNING: include this header after ALL header files since it will redefine symbols

#ifdef DEBUG
#include <stdio.h>
#endif

// Debugging macros
#ifdef DEBUG
#define GetFrameTime() DELTA_TIME;
#define PRINT_FPS(x,y) DrawFPS((x), (y))
#define dbg_print(...) printf(__VA_ARGS__)
#else 
#define PRINT_FPS(x,y)
#define dbg_print(...)
#endif

#endif
