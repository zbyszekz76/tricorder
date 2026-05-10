#pragma once

#include "../drivers/lgfx_display.h"

// =================================================
// ===================== COLORS ====================
// =================================================

#define UI_COLOR_BG       TFT_BLACK
#define UI_COLOR_TITLE    TFT_CYAN
#define UI_COLOR_LABEL    TFT_WHITE
#define UI_COLOR_VALUE    TFT_GREEN

// =================================================
// ===================== API =======================
// =================================================

void ui_draw_title(
    LGFX_Sprite* sprite,
    int x,
    int y,
    const char* text
);

void ui_draw_label(
    LGFX_Sprite* sprite,
    int x,
    int y,
    const char* text
);

void ui_draw_float(
    LGFX_Sprite* sprite,
    int x,
    int y,
    const char* label,
    float value
);