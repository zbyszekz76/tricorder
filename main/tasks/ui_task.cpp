#include "ui_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include <math.h>

#include "../drivers/lgfx_display.h"

#include "../common/shared_data.h"

#include "../ui/ui_helpers.h"

// =================================================
// ===================== TAG =======================
// =================================================

static const char *TAG = "UI_TASK";

static constexpr float DEG_TO_RAD = 0.01745329252f;

// =================================================
// ===================== SPRITE ====================
// =================================================

static LGFX_Sprite sprite(&tft);

// =================================================
// ===================== TASK ======================
// =================================================

void ui_task(void *arg)
{
    ESP_LOGI(TAG, "UI task started");

    // ================= TFT INIT =================

    tft.init();

    tft.setRotation(1);

    // ================= SPRITE =================

    sprite.createSprite(320, 240);

    sprite.setTextColor(TFT_GREEN);

    sprite.setTextSize(2);

    uint32_t frame_count = 0;

    uint32_t fps = 0;

    TickType_t last_fps_tick = xTaskGetTickCount();

    TickType_t lastWakeTime = xTaskGetTickCount();

    while (1)
    {
        // ================= CLEAR =================

        sprite.fillSprite(TFT_BLACK);

        // ================= HORIZON =================

        float pitch = g_imu_data.pitch;
        float roll  = g_imu_data.roll;

        int cx = 160;
        int cy = 120;

        // pitch offset
        int pitch_offset = pitch * 2;

        // horizon vector
        float rad = roll * DEG_TO_RAD;

        int x1 = cx - 200 * cos(rad);
        int y1 = cy - 200 * sin(rad) + pitch_offset;

        int x2 = cx + 200 * cos(rad);
        int y2 = cy + 200 * sin(rad) + pitch_offset;

        // sky
        sprite.fillRect(
            0,
            0,
            320,
            cy + pitch_offset,
            TFT_BLUE
        );

        // ground
        sprite.fillRect(
            0,
            cy + pitch_offset,
            320,
            240,
            TFT_BROWN
        );

        // horizon line
        sprite.drawLine(
            x1,
            y1,
            x2,
            y2,
            TFT_WHITE
        );

        // center marker
        sprite.drawCircle(
            cx,
            cy,
            5,
            TFT_WHITE
        );

        ui_draw_float(
            &sprite,
            UI_MARGIN_X,
            30 + UI_LINE_H * 0,
            "PITCH",
            g_imu_data.pitch
        );

        ui_draw_float(
            &sprite,
            UI_MARGIN_X,
            30 + UI_LINE_H * 1,
            "ROLL",
            g_imu_data.roll
        );       

        // ================= TITLE =================

    ui_draw_title(
        &sprite,
        10,
        10,
        "TRICORDER CORE"
    );

    ui_draw_float(
        &sprite,
        10,
        70,
        "AX",
        g_imu_data.ax
    );

    ui_draw_float(
        &sprite,
        10,
        90,
        "AY",
        g_imu_data.ay
    );

    ui_draw_float(
        &sprite,
        10,
        110,
        "AZ",
        g_imu_data.az
    );

    ui_draw_label(
        &sprite,
        10,
        180,
        "FPS"
    );

    ui_draw_float(
        &sprite,
        80,
        180,
        "",
        (float)fps
    );

        // ================= FPS =================

        frame_count++;

        // ================= PUSH =================

        sprite.pushSprite(0, 0);

        TickType_t now = xTaskGetTickCount();

    if ((now - last_fps_tick) >= pdMS_TO_TICKS(1000))
    {
        fps = frame_count;

        frame_count = 0;

        last_fps_tick = now;
    }

    vTaskDelayUntil(
        &lastWakeTime,
    pdMS_TO_TICKS(16)
    );

    }
}
