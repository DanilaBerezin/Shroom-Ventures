#include "map.h"
#include "arena.h"
#include "debug.h"

static Platform InitPlatforms(
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

void InitMap(Map *map, Arena *arena) {
    map->bgTexture = LoadTexture("assets/bg0.png");
    map->bgMusic = LoadMusicStream("assets/bg-soundtrack.mp3");
    map->numPlats = 4;
    map->mapPlats = ArenaAlloc(arena, 4 * sizeof(map->mapPlats[0]));
    map->mapPlats[0] = InitPlatforms(-7500, 400, 16250, 1000, true, BROWN);
    map->mapPlats[1] = InitPlatforms(812, 250, 125, 12, true, BROWN);
    map->mapPlats[2] = InitPlatforms(312, 250, 125, 12, true, BROWN);
    map->mapPlats[3] = InitPlatforms(375, 100, 400, 12, true, BROWN);
}
