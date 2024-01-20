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
    bool prevPausePress = st->inpState->prevPausePress;
    bool currPausePress = IsKeyDown(KEY_ESCAPE);
    if (currPausePress && !prevPausePress) {
        requests |= PAUSE_UNPAUSE_REQUESTED;
    } 
    nextSys.inpState->prevPausePress = currPausePress;

    nextSys.inpState->inputRequests = requests;
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
    const float offsetCoeff = 2.0f;
    const float maxDiff = 400.0f;
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
    float diff = st->player.pos.x - st->camera.target.x;

    float length = fabsf(diff);
    if (length > minEffectLength) {
        Vector2 adjust = { 0 };
        adjust.x = diff;

        float speed = fmaxf(fractionSpeed * length, minSpeed);
        nextCam.target = Vector2Add(st->camera.target, Vector2Scale(adjust, speed * DELTA_TIME / length));
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

        // Draw the background
        Vector2 pos = { 0 };
        DrawTextureEx(st->background, pos, 0.0f, 1.0f, WHITE);
            
        // Draw stuff in camera coordinates in mode2D block 
        BeginMode2D(st->camera);
            for (uint32_t i = 0; i < st->numPlats; i++) {
                DrawRectangleRec(st->mapPlats[i].rect, st->mapPlats[i].color);
            }
            
            DrawRectangleRec(HitBox(&st->player), RED); 
        EndMode2D();

        DrawText("Move rectangle with doom keys", 10, 10, 30, BLACK);
    EndTextureMode();
}

void PlayWorldStateSound(State *st) {
    UpdateMusicStream(st->bgMusic);
    PlayPlayerSound(&st->player);
}
