#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <stdint.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#define MAX_BUILDINGS 100
#define G 800

#define MIN(a, b)                                   \
    ((a) < (b) ? (a) : (b))                         \
// WARNING: macro only works for VLAs or statically declared arrays
#define ARRAY_SIZE(arr)                             \
    (uint32_t) sizeof(arr)/sizeof(arr[0])           \

// Debugging macros
#ifdef DEBUG
    #define PRINT_FPS(x,y) DrawFPS((x), (y))
#else 
    #define PRINT_FPS(x,y)
#endif

typedef struct {
    Rectangle   rect;
    bool        block;
    Color       color;
} Platform;

Platform InitPlatforms(
    float x,
    float y,
    float width,
    float height,
    bool block,
    Color color
);

#endif
