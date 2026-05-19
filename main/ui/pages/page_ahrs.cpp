// =================================================
// ================= page_ahrs.cpp =================
// =================================================

#include "page_ahrs.h"

#include <math.h>

#include "../common/shared_data.h"

#include "../ui/ui_renderer.h"

// =================================================
// ===================== CONST =====================
// =================================================

static constexpr float DEG_TO_RAD =
    0.017453292519943295769f;

// =================================================
// ===================== PAGE ======================
// =================================================

void draw_ahrs_page(
    LGFX_Sprite* sprite
)
{
    // =================================================
    // ================= IMU DATA ======================
    // =================================================

    float pitch =
        g_imu_data.pitch;

    float roll =
        g_imu_data.roll;

    // =================================================
    // ================= CENTER ========================
    // =================================================

    int cx = 160;
    int cy = 120;

    // =================================================
    // ================= PITCH =========================
    // =================================================

    int pitch_scale = 2;

    int pitch_offset =
        pitch * pitch_scale;

    // =================================================
    // ================= ROLL ==========================
    // =================================================

    float rad =
        roll * DEG_TO_RAD;

    // =================================================
    // ================= HORIZON =======================
    // =================================================

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

    // =================================================
    // ================= SKY/GROUND ====================
    // =================================================

    draw_sky_ground(
        sprite,
        cx,
        cy,
        pitch_offset,
        rad
    );

    // =================================================
    // ================= HORIZON LINE ==================
    // =================================================

    sprite->drawLine(
        x1,
        y1,
        x2,
        y2,
        TFT_WHITE
    );

    // =================================================
    // ================= BANK SCALE ====================
    // =================================================

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

        int bx1 =
            cx + cos(a)
            * (bank_radius - 10);

        int by1 =
            cy + sin(a)
            * (bank_radius - 10);

        int bx2 =
            cx + cos(a)
            * bank_radius;

        int by2 =
            cy + sin(a)
            * bank_radius;

        sprite->drawLine(
            bx1,
            by1,
            bx2,
            by2,
            TFT_WHITE
        );
    }

    // =================================================
    // ================= ROLL POINTER ==================
    // =================================================

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

    // =================================================
    // ================= PITCH LADDER ==================
    // =================================================

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
    }

    // =================================================
    // ================= AIRCRAFT SYMBOL ===============
    // =================================================

    // wings

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

    // center

    sprite->fillCircle(
        cx,
        cy,
        3,
        TFT_YELLOW
    );

    // vertical marker

    sprite->drawLine(
        cx,
        cy - 10,
        cx,
        cy - 4,
        TFT_YELLOW
    );

    // =================================================
    // ================= CENTER CROSS ==================
    // =================================================

    sprite->drawLine(
        cx - 15,
        cy,
        cx + 15,
        cy,
        TFT_YELLOW
    );

    sprite->drawLine(
        cx,
        cy - 8,
        cx,
        cy + 8,
        TFT_YELLOW
    );

    // =================================================
    // ================= TEXT ==========================
    // =================================================

    ui_draw_title(
        sprite,
        10,
        10,
        "AHRS"
    );

    ui_draw_float(
        sprite,
        10,
        40,
        "P",
        pitch
    );

    ui_draw_float(
        sprite,
        10,
        65,
        "R",
        roll
    );
}