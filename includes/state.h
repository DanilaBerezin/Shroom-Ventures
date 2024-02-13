#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#include "raylib.h"
#include "raymath.h"
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
    
    // This is the thing external functionality will care about
    uint64_t        inputRequests;
} UserInputState;

typedef struct state {
    // System state
    float           frameTime;
    UserInputState  *inpState;

    // Application state
    enum AppState   currAppState; 

    // World state
    Map             map;
    Player          player;
    Camera2D        camera;
} State;

/*
 * TODO: explanation here
 * TODO: make these return void
 */
void NextSystemState(State *st);

/*
 * TODO: explanation here
 */
void NextWorldState(State *st);

/*
 * TODO: explanation here
 */
void DrawWorldState(State *st, RenderTexture2D rendTarg);

/*
 * TODO: explanation here
 */
void PlayWorldStateSound(State *st);

#endif
