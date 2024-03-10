#include <string.h>

#include "state.h"
#include "camera.h"
#include "debug.h"
#include "macros.h"

void NextSystemState(State *st) {
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

void DrawWorldState(State *st, RenderTexture2D rendTarg) {
	BeginTextureMode(rendTarg);
        ClearBackground(RAYWHITE);

        // Draw the background
        Map map = st->map;
        DrawTextureEx(map.bgTexture, (Vector2) { 0 }, 0.0f, 1.0f, WHITE);
            
        // Draw stuff in camera coordinates in mode2D block 
        BeginMode2D(st->camera);
            for (uint32_t i = 0; i < map.numPlats; i++) {
                DrawRectangleRec(map.mapPlats[i].rect, map.mapPlats[i].color);
            }
            
            DrawPlayer(&st->player);
        EndMode2D();

        DrawText("Move rectangle with doom keys", 10, 10, 50, BLACK);
    EndTextureMode();
}

