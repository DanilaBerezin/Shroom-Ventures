#ifndef MAP_H
#define MAP_H

#include <stdint.h>

#include "raylib.h"
#include "arena.h"

#define G 1500 

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
void MapInit(Map *map, Arena *arena);

#endif
