#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "raylib.h"

// In pixels per second
#define PLAYER_JUMP_SPEED 500.0f
#define PLAYER_HOR_SPEED 500.0f
#define PLAYER_DASH_SPEED 800.0f

// In pixels
#define PLAYER_DEFAULT_HEIGHT 40

// In seconds
#define MAX_DASH_TIME 0.15f
#define DASH_COOL_DOWN_TIME 1.0f

// Typedef to avoid circular dependency between state.h and player.h
typedef struct state State;

typedef struct player {
    // Physics
    Vector2 pos;
    Vector2 vel;
    float   width;
    float   height;

    // Movement state
    bool    isCrouch;
    bool    isDash;
    float   dashTime;
    float   walkTime;
} Player;

Rectangle HitBox(Player *play);

Player NextPlayer(State *st);

#endif
