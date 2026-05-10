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