#include "raylib.h"
#include "raymath.h"
#include <stdint.h>

#include "player.h"
#include "map.h"
#include "macros.h"
#include "debug.h"

// Functions
float GetDeltaTime(void) {
#ifdef DEBUG
    return 1.0f / 60.0f;
#else
    return GetFrameTime();
#endif
}

// NOTE: for some reason the smoothing here ends up resulting in a 
// sort of course grain appearance. Probably something internal to
// raylib though the devs deny it lol. Funnily enough casting the 
// values to ints makes it look a bit smoother imo and rounding
// doesn't seem to make a difference? Oh well I'll tinker with this
// later
Camera2D NextCamera(
    Camera2D currCam,
    Player play,
    float delta,
    int width,
    int height
) {
    Camera2D nextCam = { 0 };
    nextCam.rotation = currCam.rotation;
    nextCam.zoom = currCam.zoom;

    // PID smoothing for camera motion when you move left or right
    const float offsetCoeff = 1.5f;
    const float maxDiff = 200;
    float currOffset = currCam.offset.x;
    float offsetTarget = (float) width / 2.0f;
    if ((int) play.vel.x < 0){
        offsetTarget +=  maxDiff;
    } else if ((int) play.vel.x > 0){
        offsetTarget -= maxDiff;
    } else {
        offsetTarget = currOffset;
    }

    float offSpeed = (offsetTarget - currOffset) * offsetCoeff;
    nextCam.offset.x = currOffset + offSpeed * delta; 
    nextCam.offset.y = currCam.offset.y;
    
    // Smoothing for currCam to follow target (player in this case)
    const float minSpeed = 110;
    const float minEffectLength = 10;
    const float fractionSpeed = 3.5f;
    Vector2 diff = Vector2Subtract(play.pos, currCam.target);

    float length = Vector2Length(diff);
    if (length > minEffectLength) {
        float speed = fmaxf(fractionSpeed * length, minSpeed);
        nextCam.target = Vector2Add(currCam.target, Vector2Scale(diff, speed * delta / length));
    } else {
       nextCam.target = currCam.target;
    }

    return nextCam;
}

int main(void) {
    // Initializing application stuff 
    const int windowWidth = 800;
    const int windowHeight = 450;
    const int gameWidth = 1600;
    const int gameHeight = 900;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);    // "VSYNC_HINT" tells the GPU to try to turn on VSYNC
    InitWindow(windowWidth, windowHeight, "Shroom Ventures!");
    SetWindowMinSize(800, 450);
    SetExitKey(KEY_NULL);                                       // Disables the default behavior of closing window on ESC key
    SetTargetFPS(60);

    // Initializing the renderer, this thing allows us to stretch and resize screen while scaling the graphics and maintaining aspect ratio
    RenderTexture2D rendTarg = LoadRenderTexture(gameWidth, gameHeight);
    SetTextureFilter(rendTarg.texture, TEXTURE_FILTER_POINT); 
    
    // Map platforms
    Platform mapPlat[] = { InitPlatforms(-6000, 320, 13000, 8000, true, GRAY),
                           InitPlatforms(650, 200, 100, 10, true, GRAY),
                           InitPlatforms(250, 200, 100, 10, true, GRAY),
                           InitPlatforms(300, 100, 400, 10, true, GRAY), };

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
    play.pos.x = 400;
    play.pos.y = gnd.rect.y - 50;
    play.width = 40; 
    play.height = PLAYER_DEFAULT_HEIGHT;
    play.vel.y = 0.0f;
    play.isCrouch = false;
    play.isDash = false;
    play.dashTime = 0.0f;

    // Camera
    Camera2D camera = { 0 };
    camera.target = play.pos;
    // By default, the camera will position the target at the upper left hand corner (the origin), 
    // this offset allows you to move the camera so that it is centered on the target 
    camera.offset.x = (float) gameWidth / 2.0f;
    camera.offset.y = (float) gameHeight / 2.0f;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Main game loop
    while (!WindowShouldClose()){
        // Update state here:
        float delta = GetDeltaTime();
        play = NextPlayer(play, mapPlat, ARRAY_SIZE(mapPlat), delta);
        camera = NextCamera(camera, play, delta, gameWidth, gameHeight);

        // Side effects of state go here:
        BeginTextureMode(rendTarg);
        ClearBackground(RAYWHITE);
            
        // Draw stuff in camera coordinates in mode2D block 
        BeginMode2D(camera);
        for (uint32_t i = 0; i < MAX_BUILDINGS; i++) {
            DrawRectangleRec(builds[i], buildCol[i]);
        }
        
        for (uint32_t i = 0; i < ARRAY_SIZE(mapPlat); i++) {
            DrawRectangleRec(mapPlat[i].rect, mapPlat[i].color);
        }
        
        DrawRectangleRec(RectFromPlayer(play), RED); 
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
        dest.x = 0.5f * ((float) GetScreenWidth() - (float) gameWidth * scale);
        dest.y = 0.5f * ((float) GetScreenHeight() - (float) gameHeight * scale);
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
