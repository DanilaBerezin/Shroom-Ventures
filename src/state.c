#include "state.h"
#include "raymath.h"
#include "debug.h"
#include "macros.h"

State NextSystemState(State *st) {
    State nextSys = *st;
    nextSys.frameTime = GetFrameTime();

    // Request logic begins here
    uint64_t requests = NO_REQUESTS;
    
    // Pause and unpause request logic
    bool prevPausePress = st->userState->prevPausePress;
    bool currPausePress = IsKeyDown(KEY_ESCAPE);
    if (currPausePress && !prevPausePress) {
        requests |= PAUSE_UNPAUSE_REQUESTED;
    } 
    nextSys.userState->prevPausePress = currPausePress;

    nextSys.userState->inputRequests = requests;
    return nextSys;
}

// NOTE: for some reason the smoothing here ends up resulting in a 
// sort of course grain appearance. Probably something internal to
// raylib though the devs deny it lol. Funnily enough casting the 
// values to ints makes it look a bit smoother imo and rounding
// doesn't seem to make a difference? Oh well I'll tinker with this
// later
static Camera2D NextCamera(State *st) {
    Camera2D nextCam = { 0 };
    nextCam.rotation = st->camera.rotation;
    nextCam.zoom = st->camera.zoom;

    // PID smoothing for camera motion when you move left or right
    const float offsetCoeff = 1.5f;
    const float maxDiff = 200;
    float currOffset = st->camera.offset.x;
    float offsetTarget = (float) GAME_WIDTH / 2.0f;
    if ((int) st->player.vel.x < 0){
        offsetTarget +=  maxDiff;
    } else if ((int) st->player.vel.x > 0){
        offsetTarget -= maxDiff;
    } else {
        offsetTarget = currOffset;
    }

    float offSpeed = (offsetTarget - currOffset) * offsetCoeff;
    nextCam.offset.x = currOffset + offSpeed * DELTA_TIME; 
    nextCam.offset.y = st->camera.offset.y;
    
    // Smoothing for currCam to follow target (player in this case)
    const float minSpeed = 110;
    const float minEffectLength = 10;
    const float fractionSpeed = 3.5f;
    Vector2 diff = Vector2Subtract(st->player.pos, st->camera.target);

    float length = Vector2Length(diff);
    if (length > minEffectLength) {
        float speed = fmaxf(fractionSpeed * length, minSpeed);
        nextCam.target = Vector2Add(st->camera.target, Vector2Scale(diff, speed * DELTA_TIME / length));
    } else {
       nextCam.target = st->camera.target;
    }

    return nextCam;
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
            
        // Draw stuff in camera coordinates in mode2D block 
        BeginMode2D(st->camera);
            for (uint32_t i = 0; i < st->numBuilds; i++) {
                DrawRectangleRec(st->builds[i], st->buildCols[i]);
            }
            
            for (uint32_t i = 0; i < st->numPlats; i++) {
                DrawRectangleRec(st->mapPlats[i].rect, st->mapPlats[i].color);
            }
            
            DrawRectangleRec(HitBox(&st->player), RED); 
        EndMode2D();

        DrawText("Move rectangle with doom keys", 10, 10, 30, DARKGRAY);
    EndTextureMode();

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
