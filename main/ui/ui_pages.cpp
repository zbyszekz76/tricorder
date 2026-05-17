#include "ui_pages.h"

// =================================================
// ===================== STATE =====================
// =================================================

static UiPage current_page =
    PAGE_AHRS;

// =================================================
// ===================== NEXT ======================
// =================================================

void next_page(void)
{
    current_page =
        (UiPage)((current_page + 1) % 3);
}

void prev_page(void)
{
    int page =
        (int)current_page;

    page--;

    if (page < 0)
    {
        page =
            PAGE_COUNT - 1;
    }

    current_page =
        (UiPage)page;

}

// =================================================
// ===================== GET =======================
// =================================================

UiPage get_current_page(void)
{
    return current_page;
}