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

Player NextPlayer(
    Player currPlay, 
    Platform *mapPlat, 
    uint32_t numPlatforms, 
    float delta
) {
    Player nextPlay = { 0 };
    nextPlay.width = currPlay.width;

    // Check for collisions
    struct { 
        Platform plat; 
        bool willColl; 
    } colInfo = { 0 };

    for (uint32_t i = 0; i < numPlatforms; i++) {
        Platform plat = mapPlat[i];
        
        // Calculate what the next rectangle's position would be assuming no collision happened
        Rectangle currRect = RectFromPlayer(currPlay);
        Rectangle nextRect = currRect;
        nextRect.y = currRect.y + (currPlay.vel.y + G * delta) * delta;

        float currRectBot = currRect.y + currRect.height;
        float platTop = plat.rect.y;
        if (currRectBot <= platTop && CheckCollisionRecs(nextRect, plat.rect)) {
            colInfo.plat = plat;
            colInfo.willColl = true;
            break;
        }
    }
    
    // Determine x-component of speed if next player won't be still
    float xSpeed;
    if (IsKeyDown(KEY_LEFT_SHIFT) && !currPlay.isCrouch
                                  && colInfo.willColl) {
        xSpeed = PLAYER_SPRINT_SPEED;
    } else {
        xSpeed = PLAYER_HOR_SPEED;
    }

    // Calculate x-component of velocity
    if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
        nextPlay.vel.x = -xSpeed;
    } else if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
        nextPlay.vel.x = xSpeed;
    } else if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
        nextPlay.vel.x = 0;
    } else {
        nextPlay.vel.x = currPlay.vel.x;
    }

    // Calculate y-component of velocity
    if (IsKeyDown(KEY_W) && colInfo.willColl) {
        nextPlay.vel.y = -PLAYER_JUMP_SPEED;
    } else if (!IsKeyDown(KEY_W) && colInfo.willColl) {
        nextPlay.vel.y = 0;
    } else {
        nextPlay.vel.y = currPlay.vel.y + G * delta;
    }

    // Calculate x-component of position. Coupled to x-component of velocity, so has
    // to be calucated after x-component of velocity is calculated
    nextPlay.pos.x = currPlay.pos.x + nextPlay.vel.x * delta;

    // Calculate y-component of position. Coupled to y-component of velocity, so has
    // to be calculated after y-component of velocity is calculated
    if (colInfo.willColl) {
        Platform colPlat = colInfo.plat;
        nextPlay.pos.y = colPlat.rect.y - currPlay.height;
    } else {
        nextPlay.pos.y = currPlay.pos.y + nextPlay.vel.y * delta;
    }

    // Crouch logic changed y-position, so is unfortunately coupled to y-position
    // calculations and has to be run after y-position is calculated
    float crouchHeight = PLAYER_DEFAULT_HEIGHT / 2;
    if (IsKeyDown(KEY_LEFT_CONTROL) && !currPlay.isCrouch) { 
        nextPlay.pos.y = nextPlay.pos.y + crouchHeight;
        nextPlay.height = crouchHeight;
        nextPlay.isCrouch = true;
    } else if (!IsKeyDown(KEY_LEFT_CONTROL) && currPlay.isCrouch) {
        nextPlay.pos.y = nextPlay.pos.y - crouchHeight;
        nextPlay.height = PLAYER_DEFAULT_HEIGHT;
    } else {
        nextPlay.height = currPlay.height;
        nextPlay.isCrouch = currPlay.isCrouch;
    }

    return nextPlay;
}


