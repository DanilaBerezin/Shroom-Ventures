#include "map.h"

Platform InitPlatforms(
    float x,
    float y,
    float width,
    float height,
    bool block,
    Color color
) {
    Platform plat = { 0 };
    plat.rect.x = x;
    plat.rect.y = y;
    plat.rect.width = width;
    plat.rect.height = height;
    plat.block = block;
    plat.color = color;
    return plat;
}


