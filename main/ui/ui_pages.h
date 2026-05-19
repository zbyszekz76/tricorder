#pragma once

enum UiPage
{
    PAGE_MENU = 0,

    PAGE_AHRS,
    PAGE_CLOCK,
    PAGE_SYSTEM,
    PAGE_IMU_DEBUG,
    PAGE_GAME,

    PAGE_COUNT
};

UiPage get_current_page();

void set_current_page(UiPage page);

void next_page();
void prev_page();