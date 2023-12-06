#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "raylib.h"
#include "state.h"
#include "map.h"

// In pixels per second
#define PLAYER_JUMP_SPEED 500.0f
#define PLAYER_HOR_SPEED 400.0f
#define PLAYER_SPRINT_SPEED 800.0f

// In pixels
#define PLAYER_DEFAULT_HEIGHT 40

// In seconds
#define MAX_DASH_TIME 0.15f
#define DASH_COOL_DOWN_TIME 1.0f

typedef struct player {
    // Physics
    Vector2 pos;
    Vector2 vel;
    float width;
    float height;

    // Movement state
    bool isCrouch;
    bool isDash;
    float dashTime;
    float walkTime;
} Player;

Rectangle RectFromPlayer(Player play);

Player NextPlayer(State st);

#endif
