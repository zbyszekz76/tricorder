#include "ui_menu.h"

static int selected_tile = 0;

static const int TILE_COUNT = 5;

// =================================================
// ===================== GET =======================
// =================================================

int get_selected_tile()
{
    return selected_tile;
}

// =================================================
// ===================== NEXT ======================
// =================================================

void next_tile()
{
    selected_tile++;

    if(selected_tile >= TILE_COUNT)
    {
        selected_tile = 0;
    }
}

// =================================================
// ===================== PREV ======================
// =================================================

void prev_tile()
{
    selected_tile--;

    if(selected_tile < 0)
    {
        selected_tile = TILE_COUNT - 1;
    }
}