#include "ui_pages.h"

// =================================================
// ===================== STATE =====================
// =================================================

static UiPage current_page =
    PAGE_MENU;

// =================================================
// ===================== NEXT ======================
// =================================================

void next_page()
{
    if(current_page == PAGE_IMU_DEBUG)
    {
        current_page = PAGE_AHRS;
    }
    else
    {
        current_page =
            (UiPage)(current_page + 1);
    }
}

void prev_page()
{
    if(current_page == PAGE_AHRS)
    {
        current_page = PAGE_IMU_DEBUG;
    }
    else
    {
        current_page =
            (UiPage)(current_page - 1);
    }
}

// =================================================
// ===================== GET =======================
// =================================================

void set_current_page(UiPage page)
{
    current_page = page;
}

UiPage get_current_page(void)
{
    return current_page;
}