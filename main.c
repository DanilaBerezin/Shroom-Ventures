#include "raylib.h"
#include "raymath.h"
#include <stdint.h>
#include <stdio.h>

#define KEY_NONE 0
#define MAX_BUILDINGS 100
#define G 800
#define PLAYER_JUMP_SPEED 450.0f
#define PLAYER_HOR_SPEED 200.0f

// WARNING: macro only works for VLAs or statically declared arrays
#define NUM_ELS(arr)                                \
    (uint32_t) sizeof(arr)/sizeof(arr[0])           \

// Debugging macros
#ifdef DEBUG
// Raises a SIGTRAP on x86-linux, useful when debugging with gdb
#define DEBUG_BREAK __asm__("int3")         
#define DEBUG_BRK_COND(cond)                        \
    if (cond)                                       \
        DEBUG_BREAK;                                \
#else 
#define DEBUG_BREAK
#define DEBUG_BRK_COND(cond) 
#endif


typedef struct {
    Rectangle rect;
    float vertSpeed;
    bool canJump;
} Player;

typedef struct {
    Rectangle rect;
    bool blocking;
    Color color;
} Platform;

Platform InitPlatform(float x, float y, float width, float height, bool blocking, Color color){
    return (Platform) {
        .rect = (Rectangle) {
            .x = x,
            .y = y,
            .width = width,
            .height = height,
        },
        .blocking = blocking,
        .color = color
    };
}

Camera2D NextCamera(Camera2D camera, Player player, float delta, int width, int height){
    const float offsetCoeff = 0.03f;
    const float maxDiff = 200;
    float currOffset = camera.offset.x;
    float offsetTarget = (float) width/2.0f;
    if(IsKeyDown(KEY_A)){
        offsetTarget +=  maxDiff;
    }else if(IsKeyDown(KEY_D)){
        offsetTarget -= maxDiff;
    }else{
        offsetTarget = currOffset;
    }
    camera.offset.x += (offsetTarget - currOffset) * offsetCoeff;
    
    const float minSpeed = 110;
    const float minEffectLength = 10;
    const float fractionSpeed = 3.5f;
    
    Vector2 playPos = (Vector2) {
        .x = player.rect.x + player.rect.width,
        .y = player.rect.y + player.rect.height
    };
    Vector2 diff = Vector2Subtract(playPos, camera.target);
    float length = Vector2Length(diff);
    if (length > minEffectLength) {
        float speed = fmaxf(fractionSpeed*length, minSpeed);
        camera.target = Vector2Add(camera.target, Vector2Scale(diff, speed*delta/length));
    }
    

    return camera;
}

Player NextPlayer(Player currPlayer, Platform *mapPlatforms, uint32_t numPlatforms, float delta){
    if(IsKeyDown(KEY_A))
        currPlayer.rect.x -= PLAYER_HOR_SPEED * delta;
    if(IsKeyDown(KEY_D))
        currPlayer.rect.x += PLAYER_HOR_SPEED * delta;
    if(IsKeyDown(KEY_W) && currPlayer.canJump){
        currPlayer.vertSpeed = -PLAYER_JUMP_SPEED;
        currPlayer.canJump = false;
    }

    for (uint32_t i = 0; i < numPlatforms; i++){
        Platform plat = mapPlatforms[i];
        Rectangle platRect = plat.rect;
        Rectangle currRect = currPlayer.rect;
        Rectangle noBlockRect = currRect;
        noBlockRect.y += (currPlayer.vertSpeed + G*delta)*delta;
        float currRectBot = currRect.y + currRect.height;
        float platTop = platRect.y;
        if (currRectBot <= platTop && CheckCollisionRecs(noBlockRect, platRect)){
            currPlayer.vertSpeed = 0.0f;
            currPlayer.rect.y = plat.rect.y - currPlayer.rect.height;
            currPlayer.canJump = true;
            return currPlayer;
        }
    }

    currPlayer.rect.y += currPlayer.vertSpeed*delta;
    currPlayer.vertSpeed += G*delta;
    return currPlayer;
}

int main(void){
    // Initializing env
    // --------------------------------------------------------------------------------------------
    // Application stuff 
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Shroom Ventures!");
    SetExitKey(KEY_NONE);   // Disables the default behavior of closing window on ESC key
    SetTargetFPS(60);

    // Map platforms
    Platform mapPlatforms[] = { InitPlatform(-6000, 320, 13000, 8000, true, GRAY),
                                InitPlatform(650, 200, 100, 10, true, GRAY),
                                InitPlatform(250, 200, 100, 10, true, GRAY),
                                InitPlatform(300, 100, 400, 10, true, GRAY), };

    // Background buildings 
    Rectangle buildings[MAX_BUILDINGS];
    Color buildColors[MAX_BUILDINGS];
    Platform ground = mapPlatforms[0];
    uint32_t spacing = 0;
    for (uint32_t i = 0; i < MAX_BUILDINGS; i++){
        float width = (float) GetRandomValue(50,200);
        float height = (float) GetRandomValue(100,800);
        buildings[i] = (Rectangle) {
            .width = width,
            .height = height,
            .y = (float) screenHeight - ((float) screenHeight - ground.rect.y) - height,
            .x = ground.rect.x + (float) spacing,
        };

        buildColors[i] = (Color) { 
            .r = (uint8_t) GetRandomValue(200,240), 
            .g = (uint8_t) GetRandomValue(200,240), 
            .b = (uint8_t) GetRandomValue(200,250), 
            .a = 255 
        };
        
        spacing += (uint32_t) buildings[i].width;
    }

    // Player
    Player player;
    player.rect = (Rectangle) {
        .x = 400, 
        .y = ground.rect.y - 50, 
        .width = 40, 
        .height = 40 
    };
    player.vertSpeed = 0.0f;
    player.canJump = true;
        

    // Camera
    Camera2D camera;
    camera.target = (Vector2) { 
        .x = player.rect.x + player.rect.width, 
        .y = player.rect.y + player.rect.height 
    };
    // By default, the camera will position the target at the upper left hand corner (the origin), 
    // this offset allows you to move the camera so that it is centered on the target 
    camera.offset = (Vector2) { 
        .x = (float) screenWidth/2.0f, 
        .y = (float) screenHeight/2.0f 
    };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Main game loop
    // --------------------------------------------------------------------------------------------
    while (!WindowShouldClose()){
        // Update state here:
        // Move player
        float delta = GetFrameTime();
        player = NextPlayer(player, mapPlatforms, NUM_ELS(mapPlatforms), delta);

        // Make camera track player
        camera = NextCamera(camera, player, delta, screenWidth, screenHeight);

        // Draw state here:
        BeginDrawing();
    
            ClearBackground(RAYWHITE);
            
            // Draw stuff in camera coordinates here
            BeginMode2D(camera);
                // Background buildings
                for (uint32_t i = 0; i < MAX_BUILDINGS; i++)
                    DrawRectangleRec(buildings[i], buildColors[i]);
                
                // Platforms
                for (uint32_t i = 0; i < NUM_ELS(mapPlatforms); i++)
                    DrawRectangleRec(mapPlatforms[i].rect, mapPlatforms[i].color);
                
                // Player
                DrawRectangleRec(player.rect, RED); 
            EndMode2D();

            DrawText("move rectangle with doom keys", 10, 10, 30, DARKGRAY);
        
        EndDrawing();
    }

    // Tear-Down env
    // --------------------------------------------------------------------------------------------
    CloseWindow();
    return 0;
}
