#include <math.h>

#include "player.h"
#include "state.h"
#include "map.h"
#include "debug.h"

/* In pixels per second */
#define PLAYER_JUMP_SPEED 690.0f
#define PLAYER_HOR_SPEED 625.0f
#define PLAYER_DASH_SPEED 1500.0f

/* In pixels */
#define PLAYER_DEFAULT_HEIGHT 100 

/* In seconds */
#define MAX_DASH_TIME 0.15f
#define DASH_COOL_DOWN_TIME 1.0f
#define FRAME_TIME 0.2f

#define PLAYER_FRAMES 5

void PlayerInit(Player *play, Map map) {
    float playScale;
	Platform gnd = map.mapPlats[0];
    
    /* The SetTextureWrap() call is necessary to avoid weird artifact generation due 
     * to mipmap generation: https://github.com/raysan5/raylib/issues/724
     * TODO: check out making the texture dimensions a power of 2 
     */
    play->animTime = 0;
    play->frames = LoadTexture("../assets/player_frames.png");
    GenTextureMipmaps(&play->frames);
    SetTextureWrap(play->frames, TEXTURE_WRAP_CLAMP);  
    SetTextureFilter(play->frames, TEXTURE_FILTER_BILINEAR);

    play->jumpAudioTrigger = false;
    play->dashAudioTrigger = false;
    play->jumpSound = LoadSound("../assets/jump.wav");
    play->dashSound = LoadSound("../assets/woosh.wav");

    playScale = (float) PLAYER_DEFAULT_HEIGHT / (float) play->frames.height;
    play->pos.x = 500;
    play->pos.y = gnd.rect.y - (PLAYER_DEFAULT_HEIGHT + 50);
    play->width = (int) (((float) play->frames.width / PLAYER_FRAMES) * playScale); 
    play->height = PLAYER_DEFAULT_HEIGHT;
    play->vel.x = 0.0f;
    play->vel.y = 0.0f;
    
    play->pState = AIRBORNE;
    play->isCrouch = false;
    play->isDash = false;
    play->dashTime = 0.0f;
    play->walkTime = 0.0f;

}

Rectangle PlayerHitBox(const Player *play) {
    Rectangle hitbox = { 0 };

    hitbox.x = play->pos.x;
    hitbox.y = play->pos.y;
    hitbox.height = play->height;
    hitbox.width = play->width;

    return hitbox;
}

void PlayerUpdate2(State *st) {
    const Map map = st->map;
    const Player currPlay = st->player;
    const float crouchHeight = PLAYER_DEFAULT_HEIGHT / 2.0f;
    bool regMovReq = false;
    Vector2 nextVel;
    Vector2 nextPos;
    
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        regMovReq = true;
    }
    
    /* TODO: the TODO below is bullshit, just redo the calculations
     * TODO: now that I'm thinking about it, don't think this is a good idea ...
     * Do all the physics before hand, commit them if we are airborne (and not
     * about to hit a platform) or falling off a platform edge, or partially 
     * commit them when we are running/dashing 
     */
    nextVel = Vector2Add(currPlay.vel, (Vector2) { 0, G * DELTA_TIME });
    nextPos = Vector2Add(currPlay.pos, (Vector2) { nextVel.x * DELTA_TIME, nextVel.y * DELTA_TIME });

    /* Collision checking logic */
    struct { Platform plat; bool willColl; } colInfo = { 0 };
    Rectangle nextRect = { 0 };
    nextRect.x = nextPos.x;
    nextRect.y = nextPos.y;
    nextRect.width = currPlay.width;
    nextRect.height = currPlay.height;

    for (uint32_t i = 0; i < map.numPlats; i++) {
        Platform plat = map.mapPlats[i];
        
        /* Check to see if collision will occurs */
        if (CheckCollisionRecs(nextRect, plat.rect)) {
            colInfo.plat = plat;
            colInfo.willColl = true;
            break;
        }
    }

    /* Determine player state */
    switch (currPlay.pState) {
    case IDLE:
        st->player.walkTime = currPlay.walkTime + DELTA_TIME;

        if (IsKeyDown(KEY_LEFT_CONTROL) && !regMovReq) {
            st->player.pState = IDLE_CROUCH;    
            st->player.pos.y = st->player.pos.y + crouchHeight;
            st->player.height = crouchHeight;
        } else if (IsKeyDown(KEY_LEFT_CONTROL) && regMovReq) {
            st->player.pState = MOVING_CROUCH;    
            st->player.pos.y = st->player.pos.y + crouchHeight;
            st->player.height = crouchHeight;
            st->player.vel.x = IsKeyDown(KEY_A) ? -PLAYER_HOR_SPEED : PLAYER_HOR_SPEED;
            st->player.dir = IsKeyDown(KEY_A) ? FACING_LEFT : FACING_RIGHT;
        } else if (!IsKeyDown(KEY_LEFT_CONTROL) && regMovReq) {
            st->player.pState = RUNNING;
            st->player.vel.x = IsKeyDown(KEY_A) ? -PLAYER_HOR_SPEED : PLAYER_HOR_SPEED;
            st->player.dir = IsKeyDown(KEY_A) ? FACING_LEFT : FACING_RIGHT;
        } else if (st->inputReqs & JUMP_REQ) {
            st->player.pState = AIRBORNE;
            st->player.vel.y = -PLAYER_JUMP_SPEED;
            st->player.jumpAudioTrigger = true;
        } else if (IsKeyDown(KEY_LEFT_SHIFT) && 
                   currPlay.walkTime > DASH_COOL_DOWN_TIME) {
            st->player.pState = DASHING;
            st->player.walkTime = 0.0f;
            st->player.dashTime = currPlay.dashTime;
            st->player.dashAudioTrigger = true;
        }  

        return;
    case RUNNING:
        st->player.animTime = fmodf(currPlay.animTime + DELTA_TIME, PLAYER_FRAMES * FRAME_TIME);
        st->player.pos.x = nextPos.x;

        if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
            st->player.dir = FACING_RIGHT;
            st->player.vel.x = PLAYER_HOR_SPEED;
        } else if (!IsKeyDown(KEY_D) && IsKeyDown(KEY_A)) {
            st->player.dir = FACING_LEFT;
            st->player.vel.x = -PLAYER_HOR_SPEED;
        } 

        if (!colInfo.willColl) {
            st->player.pState = AIRBORNE;
            st->player.vel = nextVel;
            st->player.pos = nextPos;
        } else if (st->inputReqs & JUMP_REQ) {
            st->player.pState = AIRBORNE;
            st->player.vel.y = -PLAYER_JUMP_SPEED;
            st->player.jumpAudioTrigger = true;
        } else if (!regMovReq) {
            st->player.pState = IDLE;
        }

        break;
    case AIRBORNE:
        st->player.vel.y = nextVel.y;
        st->player.pos = nextPos;

        bool snapPlayer = false;
        float platTop = colInfo.plat.rect.y;
        if (colInfo.willColl) {
            float yPos = currPlay.pos.y;
            float playBot = yPos + currPlay.height;
            snapPlayer = (playBot <= platTop);
        }

        if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
            st->player.dir = FACING_RIGHT;
            st->player.vel.x = PLAYER_HOR_SPEED;
        } else if (!IsKeyDown(KEY_D) && IsKeyDown(KEY_A)) {
            st->player.dir = FACING_LEFT;
            st->player.vel.x = -PLAYER_HOR_SPEED;
        } else if (!regMovReq && currPlay.vel.x != 0.0f) {
            st->player.vel.x = 0;
        } 

        if (snapPlayer && regMovReq) {
            st->player.pState = RUNNING;
            st->player.vel.y = 0.0f;          
            st->player.pos.y = platTop - currPlay.height;
        } else if (snapPlayer && !regMovReq) {
            st->player.pState = IDLE;
            st->player.vel.y = 0.0f;          
            st->player.pos.y = platTop - currPlay.height;
        }

        break;
    case DASHING:
        if (currPlay.dashTime + DELTA_TIME <= MAX_DASH_TIME) {
            st->player.dashTime = currPlay.dashTime + DELTA_TIME;
        }
            
        break;
    case IDLE_CROUCH:
        break;
    case MOVING_CROUCH:
        break;
    default:
        perror("Player in some indeterminant state, should not be happening");
    }
}

void PlayerUpdate(State *st) {
    const Map map = st->map;
    const Player currPlay = st->player;

    /* Check for collisions */
    struct { 
        Platform plat; 
        bool willColl; 
    } colInfo = { 0 };
    for (uint32_t i = 0; i < map.numPlats; i++) {
        Rectangle currRect = PlayerHitBox(&currPlay);
        Platform plat = map.mapPlats[i];
        
        /* Check to see if collisions occurs */
        if (CheckCollisionRecs(currRect, plat.rect)) {
            colInfo.plat = plat;
            colInfo.willColl = true;
            break;
        }
    }

    /* Dash state logic */
    if (currPlay.dashTime + DELTA_TIME > MAX_DASH_TIME) {
        st->player.isDash = false;
        st->player.dashTime = 0.0f;
    } else if (IsKeyDown(KEY_LEFT_SHIFT) && currPlay.walkTime > DASH_COOL_DOWN_TIME) {
        st->player.isDash = true;
        st->player.dashTime = currPlay.dashTime;
        st->player.dashAudioTrigger = true;
    } else if (currPlay.isDash) {
        st->player.dashTime = currPlay.dashTime + DELTA_TIME;
    } 

    /* Determine x-component of speed */
    float xSpeed;
    if (st->player.isDash && !currPlay.isCrouch) {
        st->player.walkTime = 0.0f;
        xSpeed = PLAYER_DASH_SPEED;
    } else {
        xSpeed = PLAYER_HOR_SPEED;
        st->player.walkTime = currPlay.walkTime + DELTA_TIME;
    }

    /* Calculate x-component of velocity, coupled with dash state logic
     * Also determines player direction
     */
    if (st->player.isDash && currPlay.isDash) {
        st->player.vel.x = currPlay.vel.x;
    } else if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
        st->player.vel.x = -xSpeed;
        st->player.dir = FACING_LEFT;
    } else if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
        st->player.vel.x = xSpeed;
        st->player.dir = FACING_RIGHT;
    } else if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
        st->player.vel.x = 0;
    } 

    /* Logic to determine if we need to snap player to a platform */
    bool snapPlayer = false;
    if (colInfo.willColl) {
        float prevYPos = currPlay.pos.y - currPlay.vel.y * DELTA_TIME;
        float prevPlayBot = prevYPos + currPlay.height;
        float platTop = colInfo.plat.rect.y;
        snapPlayer = (prevPlayBot <= platTop);
    }

   /* Calculate y-component of velocity, coupled with dash state logic as well
    * TODO: use player state to determine whether a jump can occur instead
    */ 
    if ((st->inputReqs & JUMP_REQ) && 
        fabs(currPlay.vel.y) <= G * DELTA_TIME) {
        st->player.vel.y = -PLAYER_JUMP_SPEED;
        st->player.jumpAudioTrigger = true;
    } else if (snapPlayer || currPlay.isDash) {
        st->player.vel.y = 0;
    } else {
        st->player.vel.y = currPlay.vel.y + G * DELTA_TIME;
    }

    /* Calculate x-component of position. Coupled to x-component of velocity */
    st->player.pos.x = currPlay.pos.x + st->player.vel.x * DELTA_TIME;

    /* Calculate y-component of position. Coupled to y-component of velocity. */
    if (snapPlayer && st->player.vel.y >= 0) {
        Platform colPlat = colInfo.plat;
        st->player.pos.y = colPlat.rect.y - currPlay.height;
    } else {
        st->player.pos.y = currPlay.pos.y + st->player.vel.y * DELTA_TIME;
    }

    /* Crouch logic changed y-position, so is unfortunately coupled to y-position
     * calculations and has to be run after y-position is calculated
     */
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

    /* Very simple implementation for now */
    if (st->player.vel.x != 0) {
        st->player.animTime = fmodf(currPlay.animTime + DELTA_TIME, PLAYER_FRAMES * FRAME_TIME);
    }
}

void PlayerDraw(Player *play) {
    uint32_t currFrame = (uint32_t) (play->animTime / FRAME_TIME);
    uint32_t frameWidth = (uint32_t) play->frames.width / PLAYER_FRAMES;
    Rectangle src, dest = PlayerHitBox(play);
    Vector2 origin = { 0 };
    
    src.x = frameWidth * currFrame; 
    src.y = 0;
    src.height = play->frames.height;
    if (play->dir == FACING_RIGHT) {
        src.width = frameWidth;
    } else {
        src.width = -((int32_t) frameWidth);
    }

    DrawTexturePro(play->frames, src, dest, origin, 0, WHITE);
}

void PlayerHandleAudioTriggers(Player *play) {
    if (play->jumpAudioTrigger) {
        PlaySound(play->jumpSound);
        play->jumpAudioTrigger = false;
    }

    if (play->dashAudioTrigger) {
        PlaySound(play->dashSound);
        play->dashAudioTrigger = false;
    }
}
