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

        // =================================================
        // ================= HORIZON DATA ==================
        // =================================================

        float pitch = g_imu_data.pitch;
        float roll  = g_imu_data.roll;

        // =================================================
        // ================= SCREEN CENTER =================
        // =================================================

        int cx = 160;
        int cy = 120;

        // =================================================
        // ================= PITCH OFFSET ==================
        // =================================================

        // ile pixeli na 1 stopień
        int pitch_scale = 2;

        int pitch_offset = pitch * pitch_scale;

        // =================================================
        // ================= ROLL ==========================
        // =================================================

        float rad = roll * DEG_TO_RAD;

        // długa linia horizonu
        int horizon_len = 250;

        int x1 = cx - horizon_len * cos(rad);
        int y1 = cy - horizon_len * sin(rad) + pitch_offset;

        int x2 = cx + horizon_len * cos(rad);
        int y2 = cy + horizon_len * sin(rad) + pitch_offset;

        // =================================================
        // ================= SKY ===========================
        // =================================================

        sprite.fillRect(
            0,
            0,
            320,
            cy + pitch_offset,
            TFT_BLUE
        );

        // =================================================
        // ================= GROUND ========================
        // =================================================

        sprite.fillRect(
            0,
            cy + pitch_offset,
            320,
            240,
            TFT_BROWN
        );

        // =================================================
        // ================= HORIZON LINE ==================
        // =================================================

        sprite.drawLine(
            x1,
            y1,
            x2,
            y2,
            TFT_WHITE
        );

        // =================================================
        // ================= CENTER MARK ===================
        // =================================================

        sprite.drawLine(
            cx - 20,
            cy,
            cx + 20,
            cy,
            TFT_YELLOW
        );

        sprite.drawLine(
            cx,
            cy - 10,
            cx,
            cy + 10,
            TFT_YELLOW
        );

        // =================================================
        // ================= TEXT ==========================
        // =================================================

        ui_draw_title(
            &sprite,
            10,
            10,
            "TRICORDER AHRS"
        );

        ui_draw_float(
            &sprite,
            10,
            40,
            "PITCH",
            pitch
        );

        ui_draw_float(
            &sprite,
            10,
            70,
            "ROLL",
            roll
        );

        ui_draw_float(
            &sprite,
            10,
            100,
            "FPS",
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
