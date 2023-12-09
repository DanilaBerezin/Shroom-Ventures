#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include "raylib.h"
#include "player.h"
#include "map.h"

typedef struct state {
    // System state
    float           frameTime;

    // World state
    uint32_t        numPlats;
    Platform        *mapPlats;
    uint32_t        numBuilds;
    Rectangle       *builds;
    Color           *buildCols;
    Player          player;
    Camera2D        camera;
} State;

State NextSystemState(State *st);

State NextWorldState(State *st, int gameWidth);

void DrawWorldState(State *st, RenderTexture2D, int gameWidth, int gameHeight);

#endif
