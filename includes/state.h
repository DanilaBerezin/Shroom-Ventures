#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include "raylib.h"
#include "player.h"
#include "map.h"

enum AppState {
    RUNNING,
    PAUSED
};

typedef struct state {
    // System state
    float           frameTime;

    // Application state
    enum AppState   currAppState; 

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
