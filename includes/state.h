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

// TODO: add requests for every other behavior in the game so far
#define NO_REQUESTS             (0)
#define PAUSE_UNPAUSE_REQUESTED (1)

typedef struct {
    // Internal state
    bool            prevPausePress;
    
    // Stuff external functionality will care about
    uint64_t        inputRequests;
} UserInputState;

typedef struct state {
    // System state
    float           frameTime;
    UserInputState  *inpState;

    // Application state
    enum AppState   currAppState; 

    // World state
    Texture2D       background;
    uint32_t        numPlats;
    Platform        *mapPlats;
    Music           bgMusic;
    Player          player;
    Camera2D        camera;
} State;

State NextSystemState(State *st);

State NextWorldState(State *st);

void DrawWorldState(State *st, RenderTexture2D rendTarg);

void PlayWorldStateSound(State *st);

#endif
