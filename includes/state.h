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
#define NO_REQ                  (0)
#define TOGGLE_PAUSE_REQ        (0x1UL << 0)
#define JUMP_REQ                (0x1UL << 1)
#define TOGGLE_FULLSCREEN_REQ   (0x1UL << 2)

typedef struct state {
    // System state
    float           frameTime;
    uint64_t        inputReqs;

    // Application state
    enum AppState   currAppState; 
    bool            isFullscreen;
    int             prevScreenWidth;
    int             prevScreenHeight;

    // World state
    Map             map;
    Player          player;
    Camera2D        camera;
} State;

/*
 * TODO: explanation here
 */
void NextSystemState(State *st);

/*
 * TODO: explanation here
 */
void DrawWorldState(State *st, RenderTexture2D rendTarg);

#endif
