#include "map.h"
#include "arena.h"
#include "debug.h"

static Platform CreatePlatform(
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

void MapInit(Map *map, Arena *arena) {
    map->bgTexture = LoadTexture("assets/bg0.png");

    map->bgMusic = LoadMusicStream("assets/bg-soundtrack.mp3");
    SetMusicVolume(map->bgMusic, 0.25f);

    map->numPlats = 4;
    map->mapPlats = ArenaAlloc(arena, 4 * sizeof(map->mapPlats[0]));
    map->mapPlats[0] = CreatePlatform(-7500, 400, 16250, 1000, true, BROWN);
    map->mapPlats[1] = CreatePlatform(812, 250, 125, 12, true, BROWN);
    map->mapPlats[2] = CreatePlatform(312, 250, 125, 12, true, BROWN);
    map->mapPlats[3] = CreatePlatform(375, 100, 400, 12, true, BROWN);

    PlayMusicStream(map->bgMusic);
}
