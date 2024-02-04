#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include "raylib.h"
#include "arena.h"

#define G 1000

typedef struct {
    Rectangle   rect;
    bool        block;
    Color       color;
} Platform;

typedef struct map {
	Texture2D 	bgTexture;
	Music		bgMusic;
	uint32_t	numPlats;
	Platform	*mapPlats;
} Map;

/*
 * TODO: explanation here
 */
Platform InitPlatforms(
    float x,
    float y,
    float width,
    float height,
    bool block,
    Color color
);

/*
 * TODO: explanation here
 */
void InitMap(Map *map, Arena *arena);

#endif
