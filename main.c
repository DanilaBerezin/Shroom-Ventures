#include "raylib.h"
#include "raymath.h"
#include <stdint.h>
#include <stdio.h>

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
    Rectangle rect;
    float vertSpeed;
    bool canJump;
    bool isCrouch;
} Player;

typedef struct {
    Rectangle rect;
    bool blocking;
    Color color;
} Platform;

// Functions
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
    // PID smoothing for camera motion when you move left or right
    const float offsetCoeff = 0.03f;
    const float maxDiff = 200;
    float currOffset = camera.offset.x;
    float offsetTarget = (float) width/2.0f;
    if (IsKeyDown(KEY_A)){
        offsetTarget +=  maxDiff;
    }else if (IsKeyDown(KEY_D)){
        offsetTarget -= maxDiff;
    }else{
        offsetTarget = currOffset;
    }
    camera.offset.x += (offsetTarget - currOffset) * offsetCoeff;
    
    // Smoothing for camera to follow target (player in this case)
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
    float playSpeed = PLAYER_HOR_SPEED;
    if (IsKeyDown(KEY_LEFT_SHIFT) && !currPlayer.isCrouch && currPlayer.canJump)
        playSpeed = PLAYER_SPRINT_SPEED;

    if (IsKeyDown(KEY_A))
        currPlayer.rect.x -= playSpeed * delta;

    if (IsKeyDown(KEY_D))
        currPlayer.rect.x += playSpeed * delta;

    if (IsKeyDown(KEY_W) && currPlayer.canJump){
        currPlayer.vertSpeed = -PLAYER_JUMP_SPEED;
        currPlayer.canJump = false;
    }

    if (IsKeyDown(KEY_LEFT_CONTROL)){
        if (!currPlayer.isCrouch){
            float crouchHeight = PLAYER_DEFAULT_HEIGHT / 2; 
            currPlayer.rect.height = crouchHeight;
            currPlayer.rect.y += crouchHeight;
            currPlayer.isCrouch = true;
        }
    } else if (currPlayer.isCrouch){
        currPlayer.rect.height = PLAYER_DEFAULT_HEIGHT;
        currPlayer.rect.y -= PLAYER_DEFAULT_HEIGHT / 2;
        currPlayer.isCrouch = false;
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
    // Application stuff 
    const int windowWidth = 800;
    const int windowHeight = 450;
    const int gameScreenWidth = 1600;
    const int gameScreenHeight = 900;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT); // "VSYNC_HINT" tells the GPU to try to turn on VSYNC
    InitWindow(windowWidth, windowHeight, "Shroom Ventures!");
    SetWindowMinSize(800, 450);
    SetExitKey(KEY_NULL);                                    // Disables the default behavior of closing window on ESC key
    SetTargetFPS(60);

    // Initializing the renderer, this thing allows us to stretch and resize screen while scaling the graphics and maintaining aspect ratio
    RenderTexture2D renderTarget = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(renderTarget.texture, TEXTURE_FILTER_POINT); 
    
    // Game objects
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
            .y = (float) gameScreenHeight - ((float) gameScreenHeight - ground.rect.y) - height,
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
        .height = PLAYER_DEFAULT_HEIGHT 
    };
    player.vertSpeed = 0.0f;
    player.canJump = true;
    player.isCrouch = false;
        

    // Camera
    Camera2D camera;
    camera.target = (Vector2) { 
        .x = player.rect.x + player.rect.width, 
        .y = player.rect.y + player.rect.height 
    };
    // By default, the camera will position the target at the upper left hand corner (the origin), 
    // this offset allows you to move the camera so that it is centered on the target 
    camera.offset = (Vector2) { 
        .x = (float) gameScreenWidth/2.0f, 
        .y = (float) gameScreenHeight/2.0f 
    };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Main game loop
    while (!WindowShouldClose()){
        // Update state here:
        // Move player
        float delta = GetFrameTime();
        player = NextPlayer(player, mapPlatforms, ARRAY_SIZE(mapPlatforms), delta);

        // Make camera track player
        camera = NextCamera(camera, player, delta, gameScreenWidth, gameScreenHeight);

        // Draw state here:
        BeginTextureMode(renderTarget);
    
            ClearBackground(RAYWHITE);
            
            // Draw stuff in camera coordinates here
            BeginMode2D(camera);
                // Background buildings
                for (uint32_t i = 0; i < MAX_BUILDINGS; i++)
                    DrawRectangleRec(buildings[i], buildColors[i]);
                
                // Platforms
                for (uint32_t i = 0; i < ARRAY_SIZE(mapPlatforms); i++)
                    DrawRectangleRec(mapPlatforms[i].rect, mapPlatforms[i].color);
                
                // Player
                DrawRectangleRec(player.rect, RED); 
            EndMode2D();

            DrawText("move rectangle with doom keys", 10, 10, 30, DARKGRAY);
        
        EndTextureMode();

        // This will draw the texture
        BeginDrawing();
            ClearBackground(BLACK);

            float scale = MIN((float)GetScreenWidth()/(float)gameScreenWidth, (float)GetScreenHeight()/(float)gameScreenHeight);
            Rectangle src = (Rectangle) {
                .x = 0.0f,
                .y = 0.0f,
                .width = (float) renderTarget.texture.width,
                .height = (float) -renderTarget.texture.height,
            };
            // This destination rect will scale the screen while keeping it centered
            Rectangle dest = (Rectangle) {
                .x = 0.5f*((float)GetScreenWidth() - ((float)gameScreenWidth*scale)),
                .y = 0.5f*((float)GetScreenHeight() - ((float)gameScreenHeight*scale)),
                .width = (float) gameScreenWidth*scale,
                .height = (float) gameScreenHeight*scale,
            };
            Vector2 offset = (Vector2){
                .x = 0.0f,
                .y = 0.0f,
            };

            DrawTexturePro(renderTarget.texture, src, dest, offset, 0.0f, WHITE);
            PRINT_FPS(GetScreenWidth() - 75, GetScreenHeight() - 20);
        EndDrawing();
    }

    // Tear-Down env
    UnloadRenderTexture(renderTarget);
    CloseWindow();
    return 0;
}
