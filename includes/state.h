#ifndef STATE_H
#define STATE_H

#include "raylib.h"
#include <stdint.h>

#include "player.h"
#include "map.h"
#include "debug.h"

#define DELTA_TIME (1.0f / 60.0f)

typedef struct {
    float delta;
    uint32_t numPlats;
    Platform *mapPlats;
    Player play;
    Camera2D camera;
} WorldState;

void UpdateDeltaTime(WorldState *state);

void UpdateState(WorldState *state);

void DrawState(WorldState *state);

#endif
