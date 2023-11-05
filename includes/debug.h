#ifndef DEBUG_H
#define DEBUG_G

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
