// =================================================
// ================= ui_renderer.cpp ===============
// =================================================
#include "ui_helpers.h"
#include "ui_renderer.h"



#include <math.h>

static constexpr float DEG_TO_RAD =
    0.017453292519943295769f;

// =================================================
// ================= SKY / GROUND ==================
// =================================================

void draw_sky_ground(
    LGFX_Sprite* sprite,
    int x1,
    int y1,
    int x2,
    int y2
)
{
    // =================================================
    // ================= SKY ===========================
    // =================================================

    sprite->fillTriangle(
        0,
        0,

        320,
        0,

        x1,
        y1,

        TFT_BLUE
    );

    sprite->fillTriangle(
        320,
        0,

        x2,
        y2,

        x1,
        y1,

        TFT_BLUE
    );

    // =================================================
    // ================= GROUND ========================
    // =================================================

    sprite->fillTriangle(
        0,
        240,

        320,
        240,

        x1,
        y1,

        TFT_BROWN
    );

    sprite->fillTriangle(
        320,
        240,

        x2,
        y2,

        x1,
        y1,

        TFT_BROWN
    );
}

// =================================================
// ================= HORIZON =======================
// =================================================

void draw_horizon(
    LGFX_Sprite* sprite,
    int cx,
    int cy,
    float roll,
    int pitch_offset
)
{
    float rad =
        roll * DEG_TO_RAD;

    int horizon_len = 260;

    int x1 =
        cx - horizon_len * cos(rad);

    int y1 =
        cy - horizon_len * sin(rad)
        + pitch_offset;

    int x2 =
        cx + horizon_len * cos(rad);

    int y2 =
        cy + horizon_len * sin(rad)
        + pitch_offset;

    sprite->drawLine(
        x1,
        y1,
        x2,
        y2,
        TFT_WHITE
    );
}

// =================================================
// ================= BANK SCALE ====================
// =================================================

void draw_bank_scale(
    LGFX_Sprite* sprite,
    int cx,
    int cy,
    float roll
)
{
    int bank_radius = 90;

    sprite->drawCircle(
        cx,
        cy,
        bank_radius,
        TFT_DARKGREY
    );

    for (int angle = -60;
        angle <= 60;
        angle += 15)
    {
        float a =
            (angle - 90)
            * DEG_TO_RAD;

        int x1 =
            cx + cos(a)
            * (bank_radius - 10);

        int y1 =
            cy + sin(a)
            * (bank_radius - 10);

        int x2 =
            cx + cos(a)
            * bank_radius;

        int y2 =
            cy + sin(a)
            * bank_radius;

        sprite->drawLine(
            x1,
            y1,
            x2,
            y2,
            TFT_WHITE
        );
    }

    float pointer_angle =
        (roll - 90)
        * DEG_TO_RAD;

    int px =
        cx + cos(pointer_angle)
        * (bank_radius - 20);

    int py =
        cy + sin(pointer_angle)
        * (bank_radius - 20);

    sprite->fillTriangle(
        px,
        py,

        px - 6,
        py - 10,

        px + 6,
        py - 10,

        TFT_YELLOW
    );
}

// =================================================
// ================= PITCH LADDER ==================
// =================================================

void draw_pitch_ladder(
    LGFX_Sprite* sprite,
    int cx,
    int cy,
    float roll,
    int pitch_offset
)
{
    float rad =
        roll * DEG_TO_RAD;

    int pitch_scale = 2;

    for (int angle = -30;
        angle <= 30;
        angle += 10)
    {
        if (angle == 0)
            continue;

        int offset =
            angle * pitch_scale;

        int ly =
            cy + pitch_offset - offset;

        int lx1 = cx - 40;
        int ly1 = ly;

        int lx2 = cx + 40;
        int ly2 = ly;

        int rx1, ry1;
        int rx2, ry2;

        rotate_point(
            lx1,
            ly1,
            cx,
            cy,
            rad,
            &rx1,
            &ry1
        );

        rotate_point(
            lx2,
            ly2,
            cx,
            cy,
            rad,
            &rx2,
            &ry2
        );

        sprite->drawLine(
            rx1,
            ry1,
            rx2,
            ry2,
            TFT_WHITE
        );

        sprite->setCursor(
            cx + 50,
            ly - 5
        );

        sprite->printf(
            "%d",
            angle
        );
    }
}

// =================================================
// ================= AIRCRAFT SYMBOL ===============
// =================================================

void draw_aircraft_symbol(
    LGFX_Sprite* sprite,
    int cx,
    int cy
)
{
    sprite->drawLine(
        cx - 25,
        cy,
        cx - 5,
        cy,
        TFT_YELLOW
    );

    sprite->drawLine(
        cx + 5,
        cy,
        cx + 25,
        cy,
        TFT_YELLOW
    );

    sprite->fillCircle(
        cx,
        cy,
        3,
        TFT_YELLOW
    );

    sprite->drawLine(
        cx,
        cy - 10,
        cx,
        cy - 4,
        TFT_YELLOW
    );
}

// =================================================
// ================= OVERLAY =======================
// =================================================

void draw_overlay(
    LGFX_Sprite* sprite,
    float pitch,
    float roll,
    float fps,
    float gyro_roll,
    float gyro_pitch
)
{
    ui_draw_title(
        sprite,
        10,
        10,
        "AHRS"
    );

    // ui_draw_float(
    //     sprite,
    //     10,
    //     40,
    //     "P",
    //     pitch
    // );

    // ui_draw_float(
    //     sprite,
    //     10,
    //     65,
    //     "R",
    //     roll
    // );

    // ui_draw_float(
    //     sprite,
    //     10,
    //     90,
    //     "G-R",
    //     gyro_roll
    // );

    // ui_draw_float(
    //     sprite,
    //     10,
    //     115,
    //     "G-P",
    //     gyro_pitch
    // );

    // ui_draw_float(
    //     sprite,
    //     10,
    //     140,
    //     "FPS",
    //     fps
    // );
}