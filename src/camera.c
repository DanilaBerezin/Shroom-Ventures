#include "camera.h"
#include "state.h"
#include "player.h"

void CameraInit(Camera2D *cam, Player *play) {
	cam->target.x = play->pos.x;
    cam->target.y = 0;
    cam->offset.x = (float) GAME_WIDTH / 2.0f; 
    cam->offset.y = (float) GAME_HEIGHT / 2.0f; /* Offset allows us to center at target instead of upper-left hand corner of screen */
    cam->rotation = 0.0f;
    cam->zoom = 1.0f;
}

/* NOTE: for some reason the smoothing here ends up resulting in a 
 * sort of course grain appearance. Probably something internal to
 * raylib though the devs deny it lol. Funnily enough casting the 
 * values to ints makes it look a bit smoother imo and rounding
 * doesn't seem to make a difference? Oh well I'll tinker with this
 * later
 */
void CameraUpdate(State *st) {
    Camera2D currCam = st->camera;

    /* PID smoothing for camera motion when you move left or right */
    const float offsetCoeff = 2.0f;
    const float maxDiff = 400.0f;
    float currOffset = currCam.offset.x;
    float offsetTarget = (float) GAME_WIDTH / 2.0f;

    if ((int) st->player.vel.x < 0){
        offsetTarget +=  maxDiff;
    } else if ((int) st->player.vel.x > 0){
        offsetTarget -= maxDiff;
    } else {
        offsetTarget = currOffset;
    }

    float offSpeed = (offsetTarget - currOffset) * offsetCoeff;
    st->camera.offset.x = currOffset + offSpeed * DELTA_TIME; 
    st->camera.offset.y = currCam.offset.y;
    
	/* Player tracking, smoothing applied along x-axis */
    const float minSpeed = 110;
    const float minEffectLength = 10;
    const float fractionSpeed = 3.5f;
    float diff = st->player.pos.x - currCam.target.x;

    float length = fabsf(diff);
    if (length > minEffectLength) {
        Vector2 adjust = { 0 };
        float speed = fmaxf(fractionSpeed * length, minSpeed);

        adjust.x = diff;
        st->camera.target = Vector2Add(currCam.target, Vector2Scale(adjust, speed * DELTA_TIME / length));
    } 
	
	st->camera.target.y = st->player.pos.y + st->player.height / 2;
}
