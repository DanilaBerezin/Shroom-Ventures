#include "state.h"
#include "camera.h"
#include "debug.h"
#include "macros.h"

void StateUpdateSystem(State *st) {
    st->frameTime = GetFrameTime();

    // Request logic begins here
    uint64_t requests = NO_REQ;
    
    // Toggle pause request logic
    if (IsKeyPressed(KEY_ESCAPE)) {
        requests |= TOGGLE_PAUSE_REQ;
    } 

    // Jump request logic
    if (IsKeyPressed(KEY_W)) {
        requests |= JUMP_REQ;
    } 

    // Toggle fullscreen request logic
    if (IsKeyPressed(KEY_F11)) {
        requests |= TOGGLE_FULLSCREEN_REQ;
    }

    st->inputReqs = requests;
}

void StateDrawWorld(State *st, RenderTexture2D rendTarg) {
	BeginTextureMode(rendTarg);
     
    // Draw the background
    Map map = st->map;
    ClearBackground(RAYWHITE);
    DrawTextureEx(map.bgTexture, (Vector2) { 0 }, 0.0f, 1.1f, WHITE);
        
    // Draw stuff in camera coordinates in mode2D block 
    BeginMode2D(st->camera);

    for (uint32_t i = 0; i < map.numPlats; i++) {
        DrawRectangleRec(map.mapPlats[i].rect, map.mapPlats[i].color);
    }

    PlayerDraw(&st->player);

    EndMode2D();
    
    // The "HUD"
    DrawText("Move rectangle with doom keys", 10, 10, 50, BLACK);

    EndTextureMode();
}

