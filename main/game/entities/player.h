#pragma once
// // Aktualna pozycja statku gracza (oś Y)
// static float ship_y = 120.0f;

// // Stała pozycja X statku
// static const int ship_x = 30;

struct Player
{
    float x;
    float y;

    bool alive;
};

extern Player g_player;