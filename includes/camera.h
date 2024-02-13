#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "state.h"
#include "player.h"

/*
 * TODO: explanation here
 */
void InitCamera(Camera2D *cam, Player *play);

/*
 * TODO: explanation here
 */
void NextCamera(State *st);

#endif
