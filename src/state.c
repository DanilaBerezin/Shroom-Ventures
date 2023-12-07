#include "state.h"
#include "raymath.h"

State NextSystemState(State *st) {
    State nextSys = *st;
    nextSys.frameTime = GetFrameTime();
    return nextSys;
}

// NOTE: for some reason the smoothing here ends up resulting in a 
// sort of course grain appearance. Probably something internal to
// raylib though the devs deny it lol. Funnily enough casting the 
// values to ints makes it look a bit smoother imo and rounding
// doesn't seem to make a difference? Oh well I'll tinker with this
// later
static Camera2D NextCamera(State *st, int gameWidth) {
    Camera2D nextCam = { 0 };
    nextCam.rotation = st->camera.rotation;
    nextCam.zoom = st->camera.zoom;

    // PID smoothing for camera motion when you move left or right
    const float offsetCoeff = 1.5f;
    const float maxDiff = 200;
    float currOffset = st->camera.offset.x;
    float offsetTarget = (float) gameWidth / 2.0f;
    if ((int) st->player.vel.x < 0){
        offsetTarget +=  maxDiff;
    } else if ((int) st->player.vel.x > 0){
        offsetTarget -= maxDiff;
    } else {
        offsetTarget = currOffset;
    }

    float offSpeed = (offsetTarget - currOffset) * offsetCoeff;
    nextCam.offset.x = currOffset + offSpeed * st->delta; 
    nextCam.offset.y = st->camera.offset.y;
    
    // Smoothing for currCam to follow target (player in this case)
    const float minSpeed = 110;
    const float minEffectLength = 10;
    const float fractionSpeed = 3.5f;
    Vector2 diff = Vector2Subtract(st->player.pos, st->camera.target);

    float length = Vector2Length(diff);
    if (length > minEffectLength) {
        float speed = fmaxf(fractionSpeed * length, minSpeed);
        nextCam.target = Vector2Add(st->camera.target, Vector2Scale(diff, speed * st->delta / length));
    } else {
       nextCam.target = st->camera.target;
    }

    return nextCam;
}

State NextWorldState(State *st, int gameWidth) {
    State nextWorld = *st;
    nextWorld.player = NextPlayer(&nextWorld);
    nextWorld.camera = NextCamera(&nextWorld, gameWidth);
    return nextWorld;
}

void DrawWorldState(State *st) {
    return;
}
