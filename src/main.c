#include <stdint.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"
#include "state.h"
#include "map.h"
#include "player.h"
#include "camera.h"
#include "arena.h"
#include "debug.h"
#include "macros.h"

int main(void) {
    // Initializing application stuff 
    // TODO: add these to compile time constants
    const int windowWidth = 1600;
    const int windowHeight = 900;
    Arena arena = { 0 };
    State st = { 0 };
    
    // Graphics stuff
    //TODO: make app fullscreen by default, make default configurable
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);    
    InitWindow(windowWidth, windowHeight, "Shroom Ventures!");
    SetWindowMinSize(windowWidth, windowHeight);
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    InitAudioDevice();

    RenderTexture2D rendTarg = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);  
    SetTextureFilter(rendTarg.texture, TEXTURE_FILTER_BILINEAR); 

    CreateArena(&arena, 400);
    
    st.currAppState = RUNNING;
    InitMap(&st.map, &arena);
    InitPlayer(&st.player, st.map);
    InitCamera(&st.camera, &st.player);
    
    // Main game loop
    float accTime = 0;
    while (!WindowShouldClose()){
        NextSystemState(&st);

        // Response to changes in global configurations
        // TODO: save and restore un-fullscreened screen position (peep: https://github.com/raysan5/raylib/issues/2415#issuecomment-1092030959)
        if ((st.inputReqs & TOGGLE_FULLSCREEN_REQ) &&
            !st.isFullscreen) {
            int monitor = GetCurrentMonitor();

            st.isFullscreen = true;
            st.prevScreenWidth = GetScreenWidth();
            st.prevScreenHeight = GetScreenHeight();
            SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));

            // Fullscreen Workaround: https://github.com/raysan5/raylib/issues/3390
            BeginDrawing();
            EndDrawing();
            
            ToggleFullscreen();
        } else if ((st.inputReqs & TOGGLE_FULLSCREEN_REQ) &&
            st.isFullscreen) {
            st.isFullscreen = false;
            ToggleFullscreen();
            
            // Fullscreen Workaround: https://github.com/raysan5/raylib/issues/3390
            BeginDrawing();
            EndDrawing();
            
            SetWindowSize(st.prevScreenWidth, st.prevScreenHeight);
        }

        switch (st.currAppState) {
        case RUNNING: 
            if (st.inputReqs & TOGGLE_PAUSE_REQ) {
                st.currAppState = PAUSED;
            }

            // Fixed time step implementation, doesn't handle death-spiral case
            accTime += st.frameTime;
            while (accTime > DELTA_TIME){
                // Update state here:
                NextPlayer(&st);
                NextCamera(&st);

                accTime -= DELTA_TIME;
            }
            
            // Draw the world
            DrawWorldState(&st, rendTarg);
            
            // Play the sounds in the world
            PlayPlayerSound(&st.player);

            break;
        case PAUSED:
            if (st.inputReqs & TOGGLE_PAUSE_REQ) {
                st.currAppState = RUNNING;
            }
            
            // TODO: insert shader here and a pause menu
            break;
        }

        // Background music runs regardless of application state
        UpdateMusicStream(st.map.bgMusic);

        // This block will draw the texture
        BeginDrawing();
            ClearBackground(BLACK);
    
            float scale = MIN((float) GetScreenWidth() / (float) GAME_WIDTH, 
                              (float) GetScreenHeight() / (float) GAME_HEIGHT);
    
            Rectangle src = { 0 };
            src.width = rendTarg.texture.width;
            src.height = -rendTarg.texture.height;
    
            // This destination rect will scale the screen while keeping it centered
            Rectangle dest = { 0 };
            dest.x = 0.5f * ((float) GetScreenWidth() - (float) GAME_WIDTH * scale);
            dest.y = 0.5f * ((float) GetScreenHeight() - (float) GAME_HEIGHT * scale);
            dest.width = GAME_WIDTH * scale;
            dest.height = GAME_HEIGHT * scale;
    
            DrawTexturePro(rendTarg.texture, src, dest, (Vector2) { 0 }, 0.0f, WHITE);
    
            PRINT_FPS(GetScreenWidth() - 75, GetScreenHeight() - 20);
        EndDrawing();
    }

    // TODO: get rid of this stuff except for maybe CloseWindow()?
    UnloadTexture(st.map.bgTexture);
    UnloadMusicStream(st.map.bgMusic);
    UnloadSound(st.player.jumpSound);
    DestroyArena(&arena);
    UnloadRenderTexture(rendTarg);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
