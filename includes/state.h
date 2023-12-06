#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include "raylib.h"
#include "player.h"
#include "map.h"

typedef struct {
    // System state
    struct {
        const float delta;
        float frameTime;
    };

    // World state
    struct {
        uint32_t numPlats;
        Platform *mapPlats;
        Player player;
        Camera2D camera;
    };
} State;

State UpdateSystem(State st);

State UpdateWorld(State st);

void DrawWorld(State st);

#endif
