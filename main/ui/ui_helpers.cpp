#include "ui_helpers.h"

// =================================================
// ===================== TITLE =====================
// =================================================

void ui_draw_title(
    LGFX_Sprite* sprite,
    int x,
    int y,
    const char* text
)
{
    sprite->setTextColor(UI_COLOR_TITLE);

    sprite->setTextSize(2);

    sprite->setCursor(x, y);

    sprite->print(text);
}

// =================================================
// ===================== LABEL =====================
// =================================================

void ui_draw_label(
    LGFX_Sprite* sprite,
    int x,
    int y,
    const char* text
)
{
    sprite->setTextColor(UI_COLOR_LABEL);

    sprite->setTextSize(2);

    sprite->setCursor(x, y);

    sprite->print(text);
}

// =================================================
// ===================== FLOAT =====================
// =================================================

void ui_draw_float(
    LGFX_Sprite* sprite,
    int x,
    int y,
    const char* label,
    float value
)
{
    sprite->setTextColor(UI_COLOR_VALUE);

    sprite->setTextSize(2);

    sprite->setCursor(x, y);

    sprite->printf(
        "%s: %.2f",
        label,
        value
    );
}

void rotate_point(
    int x,
    int y,
    int cx,
    int cy,
    float angle_rad,
    int *rx,
    int *ry
)
{
    // przesunięcie do środka

    float tx = x - cx;
    float ty = y - cy;

    // rotacja

    float xr =
        tx * cos(angle_rad)
        - ty * sin(angle_rad);

    float yr =
        tx * sin(angle_rad)
        + ty * cos(angle_rad);

    // powrót

    *rx = xr + cx;
    *ry = yr + cy;
}