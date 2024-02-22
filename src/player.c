#include "player.h"
#include "state.h"
#include "map.h"
#include "debug.h"

// In pixels per second
#define PLAYER_JUMP_SPEED 600.0f
#define PLAYER_HOR_SPEED 625.0f
#define PLAYER_DASH_SPEED 1500.0f

// In pixels
#define PLAYER_DEFAULT_HEIGHT 100 

// In seconds
#define MAX_DASH_TIME 0.15f
#define DASH_COOL_DOWN_TIME 1.0f
#define FRAME_TIME 0.2f

void InitPlayer(Player *play, Map map) {
	Platform gnd = map.mapPlats[0];

    play->pos.x = 500;
    play->pos.y = gnd.rect.y - 150;
    play->width = 100; 
    play->height = PLAYER_DEFAULT_HEIGHT;
    play->vel.y = 0.0f;
    play->isCrouch = false;
    play->isDash = false;
    play->dashTime = 0.0f;
    play->walkTime = 0.0f;
    play->frames = LoadTexture("assets/player_frames.png");
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

void NextPlayer(State *st) {
    Player currPlay = st->player;
    Map map = st->map;

    // Check for collisions
    struct { 
        Platform plat; 
        bool willColl; 
    } colInfo = { 0 };
    for (uint32_t i = 0; i < map.numPlats; i++) {
        // Calculate what the next rectangle's position would be assuming no collision happened
        Rectangle currRect = HitBox(&currPlay);
        // Rectangle nextRect = currRect;
        // nextRect.y = currRect.y + (currPlay.vel.y + G * DELTA_TIME) * DELTA_TIME;
        
        // Check to see if collisions occurs
        Platform plat = map.mapPlats[i];
        // float currRectBot = currRect.y + currRect.height;
        // float platTop = plat.rect.y;
        if (CheckCollisionRecs(currRect, plat.rect)) {
            colInfo.plat = plat;
            colInfo.willColl = true;
            break;
        }
    }

    // Dash state logic
    if (currPlay.dashTime + DELTA_TIME > MAX_DASH_TIME) {
        st->player.isDash = false;
        st->player.dashTime = 0.0f;
    } else if (IsKeyDown(KEY_LEFT_SHIFT) && currPlay.walkTime > DASH_COOL_DOWN_TIME) {
        st->player.isDash = true;
        st->player.dashTime = currPlay.dashTime;
    } else if (currPlay.isDash) {
        st->player.dashTime = currPlay.dashTime + DELTA_TIME;
    } 

    // Determine x-component of speed
    float xSpeed;
    if (st->player.isDash && !currPlay.isCrouch) {
        st->player.walkTime = 0.0f;
        xSpeed = PLAYER_DASH_SPEED;
    } else {
        xSpeed = PLAYER_HOR_SPEED;
        st->player.walkTime = currPlay.walkTime + DELTA_TIME;
    }

    // Calculate x-component of velocity, coupled with dash state logic
    if (st->player.isDash && currPlay.isDash) {
        st->player.vel.x = currPlay.vel.x;
    } else if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
        st->player.vel.x = -xSpeed;
    } else if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
        st->player.vel.x = xSpeed;
    } else if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
        st->player.vel.x = 0;
    } 

    // Logic to determine if we need to snap player to a platform
    bool snapPlayer = false;
    if (colInfo.willColl) {
        float prevYPos = currPlay.pos.y - currPlay.vel.y * DELTA_TIME;
        float prevPlayBot = prevYPos + currPlay.height;
        float platTop = colInfo.plat.rect.y;
        snapPlayer = (prevPlayBot <= platTop);
    }

    // Calculate y-component of velocity, coupled with dash state logic as well
    if (IsKeyDown(KEY_W) && snapPlayer) {
        st->player.vel.y = -PLAYER_JUMP_SPEED;
        st->player.playJumpSound = true;
    } else if (snapPlayer || st->player.isDash) {
        st->player.vel.y = 0;
    } else {
        st->player.vel.y = currPlay.vel.y + G * DELTA_TIME;
    }

    // Calculate x-component of position. Coupled to x-component of velocity
    st->player.pos.x = currPlay.pos.x + st->player.vel.x * DELTA_TIME;

    // Calculate y-component of position. Coupled to y-component of velocity. 
    if (snapPlayer && st->player.vel.y >= 0) {
        Platform colPlat = colInfo.plat;
        st->player.pos.y = colPlat.rect.y - currPlay.height;
    } else {
        st->player.pos.y = currPlay.pos.y + st->player.vel.y * DELTA_TIME;
    }

    // Crouch logic changed y-position, so is unfortunately coupled to y-position
    // calculations and has to be run after y-position is calculated
    float crouchHeight = PLAYER_DEFAULT_HEIGHT / 2;
    if (IsKeyDown(KEY_LEFT_CONTROL) && !currPlay.isCrouch) { 
        st->player.pos.y = st->player.pos.y + crouchHeight;
        st->player.height = crouchHeight;
        st->player.isCrouch = true;
    } else if (!IsKeyDown(KEY_LEFT_CONTROL) && currPlay.isCrouch) {
        st->player.pos.y = st->player.pos.y - crouchHeight;
        st->player.height = PLAYER_DEFAULT_HEIGHT;
        st->player.isCrouch = false;
    } 

    // Very simple implementation for now
    if (st->player.vel.x != 0) {
        st->player.animTime = fmodf(currPlay.animTime + DELTA_TIME, 1.0f);
    }
}

void DrawPlayer(Player *play) {
    uint32_t currFrame = (uint32_t) (play->animTime / FRAME_TIME);
    uint32_t frameWidth = (uint32_t) play->frames.width / 5;
    Rectangle src, dest = HitBox(play);
    Vector2 origin = { 0 };
    
    src.x = frameWidth * currFrame; 
    src.y = 0;
    src.height = play->frames.height;
    if (play->vel.x >= 0) {
        src.width = frameWidth;
    } else {
        src.width = -((int32_t) frameWidth);
    }

    DrawTexturePro(play->frames, src, dest, origin, 0, WHITE);
}

void PlayPlayerSound(Player *play) {
    if(play->playJumpSound) {
        PlaySound(play->jumpSound);
        play->playJumpSound = false;
    }
}


