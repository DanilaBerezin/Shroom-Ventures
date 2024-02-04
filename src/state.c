#include <string.h>

#include "state.h"
#include "camera.h"
#include "debug.h"
#include "macros.h"

void InitState(State *st, Arena *arena) {
    // Map Initialization
    Platform *mapPlats = ArenaAlloc(arena, 4 * sizeof(mapPlats[0]));
    mapPlats[0] = InitPlatforms(-6000, 320, 13000, 8000, true, BROWN);
    mapPlats[1] = InitPlatforms(650, 200, 100, 10, true, BROWN);
    mapPlats[2] = InitPlatforms(250, 200, 100, 10, true, BROWN);
    mapPlats[3] = InitPlatforms(300, 100, 400, 10, true, BROWN);

    // Player Initialization
    Player play = { 0 };
    InitPlayer(&play, mapPlats);
    
	// Initializing state
    UserInputState *inpSt = ArenaAlloc(arena, sizeof(*inpSt));
    memset(inpSt, 0, sizeof(*inpSt));
    
    st->inpState = inpSt;
    st->currAppState = RUNNING;
    st->background = LoadTexture("assets/bg0.png");
    st->bgMusic = LoadMusicStream("assets/bg-soundtrack.mp3");
    st->numPlats = 4;
    st->mapPlats = mapPlats;
    st->player = play;
    InitCamera(&st->camera, &play);
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

        // Draw the background
        Vector2 pos = { 0 };
        DrawTextureEx(st->background, pos, 0.0f, 1.0f, WHITE);
            
        // Draw stuff in camera coordinates in mode2D block 
        BeginMode2D(st->camera);
            for (uint32_t i = 0; i < st->numPlats; i++) {
                DrawRectangleRec(st->mapPlats[i].rect, st->mapPlats[i].color);
            }
            
            DrawRectangleRec(HitBox(&st->player), RED); 
        EndMode2D();

        DrawText("Move rectangle with doom keys", 10, 10, 30, BLACK);
    EndTextureMode();
}

void PlayWorldStateSound(State *st) {
    UpdateMusicStream(st->bgMusic);
    PlayPlayerSound(&st->player);
}
