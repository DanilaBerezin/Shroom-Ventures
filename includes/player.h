#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "stdint.h"
#include "map.h"

#define PLAYER_JUMP_SPEED 450.0f
#define PLAYER_HOR_SPEED 200.0f
#define PLAYER_SPRINT_SPEED 400.0f
#define PLAYER_DEFAULT_HEIGHT 40

typedef struct {
    Vector2     pos;
    Vector2     vel;
    float       width;
    float       height;
    bool        isCrouch;
} Player;

Rectangle RectFromPlayer(Player play);

Player NextPlayer(
    Player currPlay, 
    Platform *mapPlat, 
    uint32_t numPlatforms, 
    float delta
);

#endif
