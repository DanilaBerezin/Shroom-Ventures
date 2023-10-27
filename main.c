#include "raylib.h"
#include "raymath.h"
#include <stdint.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#define MAX_BUILDINGS 100
#define G 800
#define PLAYER_JUMP_SPEED 450.0f
#define PLAYER_HOR_SPEED 200.0f
#define PLAYER_SPRINT_SPEED 400.0f
#define PLAYER_DEFAULT_HEIGHT 40

#define MIN(a, b) ((a)<(b) ? (a) : (b))
// WARNING: macro only works for VLAs or statically declared arrays
#define ARRAY_SIZE(arr)                             \
    (uint32_t) sizeof(arr)/sizeof(arr[0])           \

// Debugging macros
#ifdef DEBUG
    #define PRINT_FPS(x,y) DrawFPS((x), (y))
#else 
    #define PRINT_FPS(x,y)
#endif

//Types
typedef struct {
    Rectangle   rect;
    float       vertSpeed;
    bool        canJump;
    bool        isCrouch;
} Player;

typedef struct {
    Rectangle   rect;
    bool        block;
    Color       color;
} Platform;

// Functions
Platform InitPlatform(float x, float y, float width, float height, bool block, Color color) {
    Platform plat = { 0 };
    plat.rect.x = x;
    plat.rect.y = y;
    plat.rect.width = width;
    plat.rect.height = height;
    plat.block = block;
    plat.color = color;
    return plat;
}

Camera2D NextCamera(Camera2D camera, Player play, float delta, int width, int height) {
    // PID smoothing for camera motion when you move left or right
    const float offsetCoeff = 0.03f;
    const float maxDiff = 200;
    float currOffset = camera.offset.x;
    float offsetTarget = (float) width / 2.0f;
    if (IsKeyDown(KEY_A)){
        offsetTarget +=  maxDiff;
    } else if (IsKeyDown(KEY_D)){
        offsetTarget -= maxDiff;
    } else {
        offsetTarget = currOffset;
    }
    camera.offset.x += (offsetTarget - currOffset) * offsetCoeff;
    
    // Smoothing for camera to follow target (player in this case)
    const float minSpeed = 110;
    const float minEffectLength = 10;
    const float fractionSpeed = 3.5f;
    Vector2 playPos = { 0 };
    playPos.x = play.rect.x + play.rect.width;
    playPos.y = play.rect.y + play.rect.height;
    Vector2 diff = Vector2Subtract(playPos, camera.target);
    float length = Vector2Length(diff);
    if (length > minEffectLength) {
        float speed = fmaxf(fractionSpeed*length, minSpeed);
        camera.target = Vector2Add(camera.target, Vector2Scale(diff, speed*delta/length));
    }

    return camera;
}

Player NextPlayer(Player currPlay, 
                  Platform *mapPlat, 
                  uint32_t numPlatforms, 
                  float delta) {
    Player nextPlay = { 0 };
    nextPlay.rect.width = currPlay.rect.width;
    nextPlay.rect.height = currPlay.rect.height;
    
    // Sprint logic
    float playSpeed = PLAYER_HOR_SPEED;
    if (IsKeyDown(KEY_LEFT_SHIFT) && !currPlay.isCrouch 
                                  && currPlay.canJump) {
        playSpeed = PLAYER_SPRINT_SPEED;
    }
    
    // Direction logic 
    if (IsKeyDown(KEY_A)){
        nextPlay.rect.x = currPlay.rect.x - playSpeed * delta;
    } else if (IsKeyDown(KEY_D)) {
        nextPlay.rect.x = currPlay.rect.x + playSpeed * delta;
    } else {
        nextPlay.rect.x = currPlay.rect.x;
    }
    
    // Jump logic
    if (IsKeyDown(KEY_W) && currPlay.canJump){
        nextPlay.vertSpeed = -PLAYER_JUMP_SPEED;
        nextPlay.canJump = false;
    } else {
        nextPlay.vertSpeed = currPlay.vertSpeed + G * delta;
        nextPlay.canJump = currPlay.canJump;
    }

    // Collision logic
    for (uint32_t i = 0; i < numPlatforms; i++) {
        Platform plat = mapPlat[i];

        Rectangle currRect = currPlay.rect;
        Rectangle nextRect = nextPlay.rect;
        nextRect.y = currRect.y + (nextPlay.vertSpeed) * delta;

        float currRectBot = currRect.y + currRect.height;
        float platTop = plat.rect.y;
        if (currRectBot <= platTop && CheckCollisionRecs(nextRect, plat.rect)) {
            nextPlay.vertSpeed = 0.0f;
            nextPlay.rect.y = plat.rect.y - currRect.height;
            nextPlay.canJump = true;
            break;
        }

        nextPlay.rect = nextRect;
    } 

    // Crouch logic. Unfortunately because it changes the value of the y-component
    // of player position, it is coupled to the collision logic and must happen
    // after it.
    if (IsKeyDown(KEY_LEFT_CONTROL) && !currPlay.isCrouch) {
        float crouchHeight = PLAYER_DEFAULT_HEIGHT / 2; 
        nextPlay.rect.height = crouchHeight;
        nextPlay.rect.y = nextPlay.rect.y + crouchHeight;
        nextPlay.isCrouch = true;
    } else if (!IsKeyDown(KEY_LEFT_CONTROL) && currPlay.isCrouch) {
        nextPlay.rect.height = PLAYER_DEFAULT_HEIGHT;
        nextPlay.rect.y = nextPlay.rect.y - PLAYER_DEFAULT_HEIGHT / 2;
        nextPlay.isCrouch = false;
    } else {
        nextPlay.rect.height = currPlay.rect.height;
        nextPlay.isCrouch = currPlay.isCrouch;
    }
    
    return nextPlay;
}

int main(void) {
    // Initializing env
    // Application stuff 
    const int windowWidth = 800;
    const int windowHeight = 450;
    const int gameWidth = 1600;
    const int gameHeight = 900;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT); // "VSYNC_HINT" tells the GPU to try to turn on VSYNC
    InitWindow(windowWidth, windowHeight, "Shroom Ventures!");
    SetWindowMinSize(800, 450);
    SetExitKey(KEY_NULL);                                    // Disables the default behavior of closing window on ESC key
    SetTargetFPS(60);

    // Initializing the renderer, this thing allows us to stretch and resize screen while scaling the graphics and maintaining aspect ratio
    RenderTexture2D rendTarg = LoadRenderTexture(gameWidth, gameHeight);
    SetTextureFilter(rendTarg.texture, TEXTURE_FILTER_POINT); 
    
    // Game objects
    // Map platforms
    Platform mapPlat[] = { InitPlatform(-6000, 320, 13000, 8000, true, GRAY),
                           InitPlatform(650, 200, 100, 10, true, GRAY),
                           InitPlatform(250, 200, 100, 10, true, GRAY),
                           InitPlatform(300, 100, 400, 10, true, GRAY), };

    // Background buildings 
    Rectangle builds[MAX_BUILDINGS];
    Color buildCol[MAX_BUILDINGS];
    Platform gnd = mapPlat[0];
    uint32_t spacing = 0;
    for (uint32_t i = 0; i < MAX_BUILDINGS; i++){
        float width = GetRandomValue(50,200);
        float height = GetRandomValue(100,800);
        builds[i].width = width;
        builds[i].height = height;
        builds[i].y = gameHeight - (gameHeight - gnd.rect.y) - height;
        builds[i].x = gnd.rect.x + spacing;

        buildCol[i].r = GetRandomValue(200,240);
        buildCol[i].g = GetRandomValue(200,240);
        buildCol[i].b = GetRandomValue(200,250);
        buildCol[i].a = 255;
        
        spacing += (uint32_t) builds[i].width;
    }

    // Player
    Player play = { 0 };
    play.rect.x = 400;
    play.rect.y = gnd.rect.y - 50;
    play.rect.width = 40; 
    play.rect.height = PLAYER_DEFAULT_HEIGHT;
    play.vertSpeed = 0.0f;
    play.canJump = true;
    play.isCrouch = false;

    // Camera
    Camera2D camera = { 0 };
    camera.target.x = play.rect.x + play.rect.width;
    camera.target.y = play.rect.y + play.rect.height;
    // By default, the camera will position the target at the upper left hand corner (the origin), 
    // this offset allows you to move the camera so that it is centered on the target 
    camera.offset.x = gameWidth / 2.0f;
    camera.offset.y = gameHeight / 2.0f;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Main game loop
    while (!WindowShouldClose()){
        // Update state here:
        // Move player
        float delta = GetFrameTime();
        play = NextPlayer(play, mapPlat, ARRAY_SIZE(mapPlat), delta);

        // Make camera track player
        camera = NextCamera(camera, play, delta, gameWidth, gameHeight);

        // Draw state here:
        BeginTextureMode(rendTarg);
            ClearBackground(RAYWHITE);
            
            // Draw stuff in camera coordinates here
            BeginMode2D(camera);
                // Background buildings
                for (uint32_t i = 0; i < MAX_BUILDINGS; i++) {
                    DrawRectangleRec(builds[i], buildCol[i]);
                }
                
                // Platforms
                for (uint32_t i = 0; i < ARRAY_SIZE(mapPlat); i++) {
                    DrawRectangleRec(mapPlat[i].rect, mapPlat[i].color);
                }
                
                // Player
                DrawRectangleRec(play.rect, RED); 
            EndMode2D();

            DrawText("Move rectangle with doom keys", 10, 10, 30, DARKGRAY);

        EndTextureMode();

        // This will draw the texture
        BeginDrawing();
            ClearBackground(BLACK);

            float scale = MIN((float) GetScreenWidth() / (float) gameWidth, 
                              (float) GetScreenHeight() / (float) gameHeight);

            Rectangle src = { 0 };
            src.width = rendTarg.texture.width;
            src.height = -rendTarg.texture.height;

            // This destination rect will scale the screen while keeping it centered
            Rectangle dest = { 0 };
            dest.x = 0.5f * (GetScreenWidth() - gameWidth * scale);
            dest.y = 0.5f * (GetScreenHeight() - gameHeight * scale);
            dest.width = gameWidth * scale;
            dest.height = gameHeight * scale;

            Vector2 offset = { 0 };
            offset.x = 0.0f;
            offset.y = 0.0f;

            DrawTexturePro(rendTarg.texture, src, dest, offset, 0.0f, WHITE);

            PRINT_FPS(GetScreenWidth() - 75, GetScreenHeight() - 20);
        EndDrawing();
    }

    // Tear-Down env
    UnloadRenderTexture(rendTarg);
    CloseWindow();
    return 0;
}
