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

State UpdateSystem(State *st);

State UpdateWorld(State *st);

void DrawWorld(State *st);

#endif
