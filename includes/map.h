#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include "raylib.h"

#define MAX_BUILDINGS 100
#define G 1000

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
