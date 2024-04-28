#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "raylib.h"
#include "map.h"

// Typedef to avoid circular dependency between state.h and player.h
typedef struct state State;

typedef enum {
    FACING_LEFT,
    FACING_RIGHT
} PlayerDir;

typedef enum {
    CROUCHING,
    DASHING,
    IDLE,
    AIRBORNE,
    RUNNING,
} PlayerState;

typedef struct player {
    // Physics
    Vector2     pos;
    Vector2     vel;
    float       width;
    float       height;
    PlayerDir   dir;

    // Internal state
    bool        isCrouch;
    bool        isDash;
    float       dashTime;
    float       walkTime;

    // Rendering stuff
    float       animTime;
    Texture2D   frames;

    // Sound FX stuff
    bool        jumpAudioTrigger;
    Sound       jumpSound;
} Player;

/*
 * TODO: explanation here
 */
void PlayerInit(Player *play, Map map);

/*
 * TODO: explanation here
 */
Rectangle PlayerHitBox(const Player *play);

/*
 * TODO: explanation here
 */
void PlayerUpdate(State *st);

/*
 * TODO: explanation here
 */
void PlayerDraw(Player *play);

/*
 * TODO: explanation here
 */
void PlayerHandleAudioTriggers(Player *play);

#endif
