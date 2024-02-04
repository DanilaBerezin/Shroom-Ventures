#include "player.h"
#include "state.h"
#include "map.h"
#include "debug.h"

void InitPlayer(Player *play, Map map) {
	Platform gnd = map.mapPlats[0];

    play->pos.x = 400;
    play->pos.y = gnd.rect.y - 50;
    play->width = 40; 
    play->height = PLAYER_DEFAULT_HEIGHT;
    play->vel.y = 0.0f;
    play->isCrouch = false;
    play->isDash = false;
    play->dashTime = 0.0f;
    play->walkTime = 0.0f;
    play->playJumpSound = false;
    play->jumpSound = LoadSound("assets/jump.wav");
}

Rectangle HitBox(Player *play) {
    Rectangle hitbox = { 0 };
    hitbox.x = play->pos.x;
    hitbox.y = play->pos.y;
    hitbox.height = play->height;
    hitbox.width = play->width;
    return hitbox;
}

Player NextPlayer(State *st) {
    Player nextPlay = { 0 };
    Map map = st->map;

    nextPlay.width = st->player.width;
    nextPlay.jumpSound = st->player.jumpSound;

    // Check for collisions
    struct { 
        Platform plat; 
        bool willColl; 
    } colInfo = { 0 };
    for (uint32_t i = 0; i < map.numPlats; i++) {
        // Calculate what the next rectangle's position would be assuming no collision happened
        Rectangle currRect = HitBox(&st->player);
        Rectangle nextRect = currRect;
        nextRect.y = currRect.y + (st->player.vel.y + G * DELTA_TIME) * DELTA_TIME;
        
        // Check to see if collisions occur
        Platform plat = map.mapPlats[i];
        float currRectBot = currRect.y + currRect.height;
        float platTop = plat.rect.y;
        if (currRectBot <= platTop && CheckCollisionRecs(nextRect, plat.rect)) {
            colInfo.plat = plat;
            colInfo.willColl = true;
            break;
        }
    }

    // Dash state logic
    if (st->player.isDash && st->player.dashTime + DELTA_TIME > MAX_DASH_TIME) {
        nextPlay.isDash = false;
        nextPlay.dashTime = 0.0f;
    } else if (!st->player.isDash && IsKeyDown(KEY_LEFT_SHIFT)
                                && st->player.walkTime > DASH_COOL_DOWN_TIME) {
        nextPlay.isDash = true;
        nextPlay.dashTime = st->player.dashTime;
    } else if (st->player.isDash) {
        nextPlay.isDash = st->player.isDash;
        nextPlay.dashTime = st->player.dashTime + DELTA_TIME;
    } else {
        nextPlay.isDash = st->player.isDash;
        nextPlay.dashTime = st->player.dashTime;
    }

    // Determine x-component of speed
    float xSpeed;
    if (nextPlay.isDash && !st->player.isCrouch) {
        nextPlay.walkTime = 0.0f;
        xSpeed = PLAYER_DASH_SPEED;
    } else {
        xSpeed = PLAYER_HOR_SPEED;
        nextPlay.walkTime = st->player.walkTime + DELTA_TIME;
    }

    // Calculate x-component of velocity, coupled with dash state logic
    if (nextPlay.isDash && st->player.isDash) {
        nextPlay.vel.x = st->player.vel.x;
    } else if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
        nextPlay.vel.x = -xSpeed;
    } else if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
        nextPlay.vel.x = xSpeed;
    } else if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
        nextPlay.vel.x = 0;
    } else {
        nextPlay.vel.x = st->player.vel.x;
    }

    // Calculate y-component of velocity, coupled with dash state logic as well
    if (IsKeyDown(KEY_W) && colInfo.willColl) {
        nextPlay.vel.y = -PLAYER_JUMP_SPEED;
        nextPlay.playJumpSound = true;
    } else if ((!IsKeyDown(KEY_W) && colInfo.willColl)
                                  || nextPlay.isDash) {
        nextPlay.vel.y = 0;
        nextPlay.playJumpSound = st->player.playJumpSound;
    } else {
        nextPlay.vel.y = st->player.vel.y + G * DELTA_TIME;
        nextPlay.playJumpSound = st->player.playJumpSound;
    }

    // Calculate x-component of position. Coupled to x-component of velocity
    nextPlay.pos.x = st->player.pos.x + nextPlay.vel.x * DELTA_TIME;

    // Calculate y-component of position. Coupled to y-component of velocity. 
    if (colInfo.willColl) {
        Platform colPlat = colInfo.plat;
        nextPlay.pos.y = colPlat.rect.y - st->player.height;
    } else {
        nextPlay.pos.y = st->player.pos.y + nextPlay.vel.y * DELTA_TIME;
    }

    // Crouch logic changed y-position, so is unfortunately coupled to y-position
    // calculations and has to be run after y-position is calculated
    float crouchHeight = PLAYER_DEFAULT_HEIGHT / 2;
    if (IsKeyDown(KEY_LEFT_CONTROL) && !st->player.isCrouch) { 
        nextPlay.pos.y = nextPlay.pos.y + crouchHeight;
        nextPlay.height = crouchHeight;
        nextPlay.isCrouch = true;
    } else if (!IsKeyDown(KEY_LEFT_CONTROL) && st->player.isCrouch) {
        nextPlay.pos.y = nextPlay.pos.y - crouchHeight;
        nextPlay.height = PLAYER_DEFAULT_HEIGHT;
    } else {
        nextPlay.height = st->player.height;
        nextPlay.isCrouch = st->player.isCrouch;
    }

    return nextPlay;
}

void PlayPlayerSound(Player *play) {
    if(play->playJumpSound) {
        PlaySound(play->jumpSound);
        play->playJumpSound = false;
    }
}


