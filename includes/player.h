#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "raylib.h"
#include "map.h"

// Typedef to avoid circular dependency between state.h and player.h
typedef struct state State;

typedef struct player {
    // Physics
    Vector2     pos;
    Vector2     vel;
    float       width;
    float       height;

    // Internal state
    bool        isCrouch;
    bool        isDash;
    float       dashTime;
    float       walkTime;

    // Rendering stuff
    float       animTime;
    Texture2D   frames;

    // Sound FX stuff
    bool        playJumpSound;
    Sound       jumpSound;
} Player;

/*
 * TODO: explanation here
 */
void InitPlayer(Player *play, Map map);

/*
 * TODO: explanation here
 */
Rectangle HitBox(Player *play);

/*
 * TODO: explanation here
 * TODO: make this return void
 */
Player NextPlayer(State *st);

/*
 * TODO: explanation here
 */
void DrawPlayer(Player *play);

/*
 * TODO: explanation here
 */
void PlayPlayerSound(Player *play);

#endif
