#pragma once

#include "../drivers/lgfx_display.h"

bool isGameMenu();

bool isGamePlaying();

void startGame();

void initSpaceGame();

void updateSpaceGame();

void drawSpaceGame(
    LGFX_Sprite* sprite
);

void fireBullet();