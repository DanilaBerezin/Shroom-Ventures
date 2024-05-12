#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <assert.h>

// WARNING: include this header after ALL header files since it will redefine symbols

// Debugging macros
#ifdef DEBUG
#define GetFrameTime() DELTA_TIME;
#else 
#define DrawFPS(x,y)
#define printf(...)
#endif

#endif
