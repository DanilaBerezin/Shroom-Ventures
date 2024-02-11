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
    
    // Graphics stuff
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);    // "VSYNC_HINT" tells the GPU to try to turn on VSYNC
    InitWindow(windowWidth, windowHeight, "Shroom Ventures!");
    SetWindowMinSize(windowWidth, windowHeight);
    SetExitKey(KEY_NULL);                                       // Disables the default behavior of closing window on 
                                                                // ESC key
    SetTargetFPS(60);
    
    //TODO: make app fullscreen by default, make default configurable

    InitAudioDevice();

    RenderTexture2D rendTarg = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);  // RenderTexture allows us to stretch 
                                                                            // screen while keeping aspect ratio
    Arena arena = { 0 };
    UserInputState inpSt = { 0 };
    State st = { 0 };
    
    SetTextureFilter(rendTarg.texture, TEXTURE_FILTER_BILINEAR); 

    CreateArena(&arena, 400);
    assert(arena.err == false);
    
    // TODO: don't make inpSt it's own thing and don't make it a pointer
    st.inpState = &inpSt;
    st.currAppState = RUNNING;
    InitMap(&st.map, &arena);
    InitPlayer(&st.player, st.map);
    InitCamera(&st.camera, &st.player);
    
    // TODO: put this in InitMap()
    PlayMusicStream(st.map.bgMusic);
    
    // Main game loop
    float accTime = 0;
    while (!WindowShouldClose()){
        // Fixed time step implementation, doesn't handle death-spiral case
        st = NextSystemState(&st);
        switch (st.currAppState) {
        case RUNNING: 
            if (st.inpState->inputRequests & PAUSE_UNPAUSE_REQUESTED) {
                st.currAppState = PAUSED;
            }

            accTime += st.frameTime;
            while (accTime > DELTA_TIME){
                // Update state here:
                st = NextWorldState(&st);
                accTime -= DELTA_TIME;
            }

            // Side effects of state 
            DrawWorldState(&st, rendTarg);        
            break;
        case PAUSED:
            if (st.inpState->inputRequests & PAUSE_UNPAUSE_REQUESTED) {
                st.currAppState = RUNNING;
            }
            
            // TODO: insert shader here and a pause menu
            break;
        }

        // Sound logic runs regardless of application state
        PlayWorldStateSound(&st);

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
    
            Vector2 offset = { 0 };
            offset.x = 0.0f;
            offset.y = 0.0f;
    
            DrawTexturePro(rendTarg.texture, src, dest, offset, 0.0f, WHITE);
    
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
