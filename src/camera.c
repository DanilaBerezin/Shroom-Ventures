#include "raymath.h"
#include "state.h"
#include "camera.h"
#include "player.h"

void InitCamera(Camera2D *cam, Player *play) {
	cam->target.x = play->pos.x;
    cam->target.y = 0;
    // By default, the camera will position the target at the upper left hand corner (the origin), 
    // this offset allows you to move the camera so that it is centered on the target 
    cam->offset.x = (float) GAME_WIDTH / 2.0f;
    cam->offset.y = (float) GAME_HEIGHT / 2.0f;
    cam->rotation = 0.0f;
    cam->zoom = 1.0f;
}

/* 
 * NOTE: for some reason the smoothing here ends up resulting in a 
 * sort of course grain appearance. Probably something internal to
 * raylib though the devs deny it lol. Funnily enough casting the 
 * values to ints makes it look a bit smoother imo and rounding
 * doesn't seem to make a difference? Oh well I'll tinker with this
 * later
 */
Camera2D NextCamera(State *st) {
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
