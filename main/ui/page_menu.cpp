#include "page_menu.h"
#include "ui_menu.h"

// =================================================
// ===================== PAGE ======================
// =================================================

void draw_menu_page(
    LGFX_Sprite* sprite
)
{
    const char* labels[5] =
    {
        "AHRS",
        "CLOCK",
        "SYSTEM",
        "DEBUG",
        "GAME"
    };

    int selected = get_selected_tile();

    int tile_w = 130;
    int tile_h = 80;

    int start_x = 25;
    int start_y = 30;

    int spacing_x = 15;
    int spacing_y = 15;

    for(int i = 0; i < 5; i++)
    {
        int col = i % 2;
        int row = i / 2;

        int x =
            start_x +
            col * (tile_w + spacing_x);

        int y =
            start_y +
            row * (tile_h + spacing_y);

        uint16_t color =
            (i == selected)
            ? TFT_CYAN
            : TFT_DARKGREY;

        sprite->drawRoundRect(
            x,
            y,
            tile_w,
            tile_h,
            6,
            color
        );

        sprite->setTextColor(TFT_WHITE);

        sprite->setTextDatum(middle_center);

        sprite->drawString(
            labels[i],
            x + tile_w / 2,
            y + tile_h / 2
        );
    }
}