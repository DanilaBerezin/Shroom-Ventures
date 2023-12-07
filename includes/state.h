#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include "raylib.h"
#include "player.h"
#include "map.h"

typedef struct state {
    // System state
    const float     delta;
    float           frameTime;

    // World state
    uint32_t        numPlats;
    Platform        *mapPlats;
    Player          player;
    Camera2D        camera;
} State;

State NextSystemState(State *st);

State NextWorldState(State *st, int gameWidth);

void DrawWorldState(State *st);

#endif
