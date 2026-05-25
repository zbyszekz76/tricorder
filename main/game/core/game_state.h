#pragma once

enum GameState
{
    GAME_MENU,
    GAME_INTRO,
    GAME_PLAYING,
    GAME_OVER
};

bool isGameMenu();
bool isGamePlaying();

void setGameState(GameState state);
GameState getGameState();