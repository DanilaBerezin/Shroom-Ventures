#include <stdint.h>
#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include "map.h"
#include "state.h"
#include "debug.h"
#include "macros.h"

int main(void) {
    // Initializing application stuff 
    // TODO: add these to compile time constants
    const int windowWidth = 800;
    const int windowHeight = 450;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);    // "VSYNC_HINT" tells the GPU to try to turn on VSYNC
    InitWindow(windowWidth, windowHeight, "Shroom Ventures!");
    SetWindowMinSize(800, 450);
    SetExitKey(KEY_NULL);                                       // Disables the default behavior of closing window on 
                                                                // ESC key
    SetTargetFPS(60);

    // Initializing the renderer, this thing allows us to stretch and resize screen while scaling the graphics and 
    // maintaining aspect ratio
    RenderTexture2D rendTarg = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
    SetTextureFilter(rendTarg.texture, TEXTURE_FILTER_POINT); 

    // User input state
    // TODO: use an arena allocator for all these pointers and create an initialization
    // function
    UserInputState usrSt = { 0 };

    // Load background texture
    Texture2D background = LoadTexture("res/cloudy-background.png");
    
    // Map platforms
    Platform mapPlats[] = { InitPlatforms(-6000, 320, 13000, 8000, true, GRAY),
                           InitPlatforms(650, 200, 100, 10, true, GRAY),
                           InitPlatforms(250, 200, 100, 10, true, GRAY),
                           InitPlatforms(300, 100, 400, 10, true, GRAY), };
    Platform gnd = mapPlats[0];

    // Player
    Player play = { 0 };
    play.pos.x = 400;
    play.pos.y = gnd.rect.y - 50;
    play.width = 40; 
    play.height = PLAYER_DEFAULT_HEIGHT;
    play.vel.y = 0.0f;
    play.isCrouch = false;
    play.isDash = false;
    play.dashTime = 0.0f;

    // Camera
    Camera2D cam = { 0 };
    //cam.target = play.pos;
    cam.target.x = play.pos.x;
    cam.target.y = 0;
    // By default, the camera will position the target at the upper left hand corner (the origin), 
    // this offset allows you to move the camera so that it is centered on the target 
    cam.offset.x = (float) GAME_WIDTH / 2.0f;
    cam.offset.y = (float) GAME_HEIGHT / 2.0f;
    cam.rotation = 0.0f;
    cam.zoom = 1.0f;

    // Setting the state
    State st = { 0 };
    st.userState = &usrSt;
    st.background = background;
    st.currAppState = RUNNING;
    st.numPlats = ARRAY_SIZE(mapPlats);
    st.mapPlats = mapPlats;
    st.player = play;
    st.camera = cam;

    // Main game loop
    float accTime = 0;
    while (!WindowShouldClose()){
        // Fixed time step implementation, doesn't handle death-spiral case
        st = NextSystemState(&st);
        switch (st.currAppState) {
        case RUNNING: 
            // TODO: make this check a generic function
            if (st.userState->inputRequests & PAUSE_UNPAUSE_REQUESTED) {
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
            if (st.userState->inputRequests & PAUSE_UNPAUSE_REQUESTED) {
                    st.currAppState = RUNNING;
            }
            
            // TODO: insert shader here and a pause menu
            break;
        }

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

    // Tear-Down env
    UnloadTexture(background);
    UnloadRenderTexture(rendTarg);
    CloseWindow();
    return 0;
}
