#include "player.h"
#include "debug.h"
#include "macros.h"

Rectangle RectFromPlayer(Player play) {
    Rectangle playRect = { 0 };
    playRect.x = play.pos.x;
    playRect.y = play.pos.y;
    playRect.height = play.height;
    playRect.width = play.width;
    return playRect;
}

Player NextPlayer(State st) {
    Player nextPlay = { 0 };
    nextPlay.width = st.player.width;

    // Check for collisions
    struct { 
        Platform plat; 
        bool willColl; 
    } colInfo = { 0 };

    for (uint32_t i = 0; i < st.numPlats; i++) {
        Platform plat = st.mapPlats[i];
        
        // Calculate what the next rectangle's position would be assuming no collision happened
        Rectangle currRect = RectFromPlayer(st.player);
        Rectangle nextRect = currRect;
        nextRect.y = currRect.y + (st.player.vel.y + G * st.delta) * st.delta;

        float currRectBot = currRect.y + currRect.height;
        float platTop = plat.rect.y;
        if (currRectBot <= platTop && CheckCollisionRecs(nextRect, plat.rect)) {
            colInfo.plat = plat;
            colInfo.willColl = true;
            break;
        }
    }

    // Dash state logic
    if (st.player.isDash && st.player.dashTime + st.delta > MAX_DASH_TIME) {
        nextPlay.isDash = false;
        nextPlay.dashTime = 0.0f;
    } else if (!st.player.isDash && IsKeyDown(KEY_LEFT_SHIFT)
                                && st.player.walkTime > DASH_COOL_DOWN_TIME) {
        nextPlay.isDash = true;
        nextPlay.dashTime = st.player.dashTime;
    } else if (st.player.isDash) {
        nextPlay.isDash = st.player.isDash;
        nextPlay.dashTime = st.player.dashTime + st.delta;
    } else {
        nextPlay.isDash = st.player.isDash;
        nextPlay.dashTime = st.player.dashTime;
    }

    // Determine x-component of speed
    float xSpeed;
    if (nextPlay.isDash && !st.player.isCrouch) {
        nextPlay.walkTime = 0.0f;
        xSpeed = PLAYER_SPRINT_SPEED;
    } else {
        xSpeed = PLAYER_HOR_SPEED;
        nextPlay.walkTime = st.player.walkTime + st.delta;
    }

    // Calculate x-component of velocity, coupled with dash state logic
    if (nextPlay.isDash && st.player.isDash) {
        nextPlay.vel.x = st.player.vel.x;
    } else if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
        nextPlay.vel.x = -xSpeed;
    } else if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
        nextPlay.vel.x = xSpeed;
    } else if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
        nextPlay.vel.x = 0;
    } else {
        nextPlay.vel.x = st.player.vel.x;
    }

    // Calculate y-component of velocity, coupled with dash state logic as well
    if (IsKeyDown(KEY_W) && colInfo.willColl) {
        nextPlay.vel.y = -PLAYER_JUMP_SPEED;
    } else if ((!IsKeyDown(KEY_W) && colInfo.willColl)
                                  || nextPlay.isDash) {
        nextPlay.vel.y = 0;
    } else {
        nextPlay.vel.y = st.player.vel.y + G * st.delta;
    }

    // Calculate x-component of position. Coupled to x-component of velocity
    nextPlay.pos.x = st.player.pos.x + nextPlay.vel.x * st.delta;

    // Calculate y-component of position. Coupled to y-component of velocity. 
    if (colInfo.willColl) {
        Platform colPlat = colInfo.plat;
        nextPlay.pos.y = colPlat.rect.y - st.player.height;
    } else {
        nextPlay.pos.y = st.player.pos.y + nextPlay.vel.y * st.delta;
    }

    // Crouch logic changed y-position, so is unfortunately coupled to y-position
    // calculations and has to be run after y-position is calculated
    float crouchHeight = PLAYER_DEFAULT_HEIGHT / 2;
    if (IsKeyDown(KEY_LEFT_CONTROL) && !st.player.isCrouch) { 
        nextPlay.pos.y = nextPlay.pos.y + crouchHeight;
        nextPlay.height = crouchHeight;
        nextPlay.isCrouch = true;
    } else if (!IsKeyDown(KEY_LEFT_CONTROL) && st.player.isCrouch) {
        nextPlay.pos.y = nextPlay.pos.y - crouchHeight;
        nextPlay.height = PLAYER_DEFAULT_HEIGHT;
    } else {
        nextPlay.height = st.player.height;
        nextPlay.isCrouch = st.player.isCrouch;
    }

    return nextPlay;
}


