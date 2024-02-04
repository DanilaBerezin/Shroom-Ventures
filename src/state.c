#include <string.h>

#include "state.h"
#include "camera.h"
#include "debug.h"
#include "macros.h"

void InitState(State *st, Arena *arena) {
	// Initializing state
    UserInputState *inpSt = ArenaAlloc(arena, sizeof(*inpSt));
    memset(inpSt, 0, sizeof(*inpSt));
    
    st->inpState = inpSt;
    st->currAppState = RUNNING;
    InitMap(&st->map, arena);
    InitPlayer(&st->player, st->map);
    InitCamera(&st->camera, &st->player);
}

State NextSystemState(State *st) {
    State nextSys = *st;
    nextSys.frameTime = GetFrameTime();

    // Request logic begins here
    uint64_t requests = NO_REQUESTS;
    
    // Pause and unpause request logic
    bool prevPausePress = st->inpState->prevPausePress;
    bool currPausePress = IsKeyDown(KEY_ESCAPE);
    if (currPausePress && !prevPausePress) {
        requests |= PAUSE_UNPAUSE_REQUESTED;
    } 
    nextSys.inpState->prevPausePress = currPausePress;

    nextSys.inpState->inputRequests = requests;
    return nextSys;
}



State NextWorldState(State *st) {
    State nextWorld = *st;
    nextWorld.player = NextPlayer(&nextWorld);
    nextWorld.camera = NextCamera(&nextWorld);
    return nextWorld;
}

void DrawWorldState(State *st, RenderTexture2D rendTarg) {
	BeginTextureMode(rendTarg);
        ClearBackground(RAYWHITE);
        
        Map map = st->map;

        // Draw the background
        Vector2 pos = { 0 };
        DrawTextureEx(map.bgTexture, pos, 0.0f, 1.0f, WHITE);
            
        // Draw stuff in camera coordinates in mode2D block 
        BeginMode2D(st->camera);
            for (uint32_t i = 0; i < map.numPlats; i++) {
                DrawRectangleRec(map.mapPlats[i].rect, map.mapPlats[i].color);
            }
            
            DrawRectangleRec(HitBox(&st->player), RED); 
        EndMode2D();

        DrawText("Move rectangle with doom keys", 10, 10, 30, BLACK);
    EndTextureMode();
}

void PlayWorldStateSound(State *st) {
    Map map = st->map;

    UpdateMusicStream(map.bgMusic);
    PlayPlayerSound(&st->player);
}
