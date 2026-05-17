// =================================================
// ================= ui_renderer.h =================
// =================================================

#pragma once

#include <LovyanGFX.hpp>

void draw_sky_ground(
    LGFX_Sprite* sprite,
    int x1,
    int y1,
    int x2,
    int y2
);
void draw_horizon(
    LGFX_Sprite* sprite,
    int cx,
    int cy,
    float roll,
    int pitch_offset
);

void draw_bank_scale(
    LGFX_Sprite* sprite,
    int cx,
    int cy,
    float roll
);

void draw_pitch_ladder(
    LGFX_Sprite* sprite,
    int cx,
    int cy,
    float roll,
    int pitch_offset
);

void draw_aircraft_symbol(
    LGFX_Sprite* sprite,
    int cx,
    int cy
);

void draw_overlay(
    LGFX_Sprite* sprite,
    float pitch,
    float roll,
    float fps,
    float gyro_roll,
    float gyro_pitch
);