#pragma once

#include "../drivers/lgfx_display.h"

void initSpaceGame();

void updateSpaceGame();

void drawSpaceGame(
    LGFX_Sprite* sprite
);

void fireBullet();