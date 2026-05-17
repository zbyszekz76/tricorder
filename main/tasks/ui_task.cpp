#include "ui_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "../drivers/lgfx_display.h"

#include "../common/shared_data.h"

#include "../ui/ui_helpers.h"

#include <math.h>

// =================================================
// ===================== TAG =======================
// =================================================

static const char *TAG = "UI_TASK";

static constexpr float DEG_TO_RAD = 0.017453292519943295769f;

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

    // =================================================
    // ================= TFT INIT ======================
    // =================================================

    tft.init();

    tft.setRotation(3);

    // =================================================
    // ================= SMALLER SPRITE ================
    // =================================================

    sprite.createSprite(320, 240);

    sprite.setTextSize(2);

    // =================================================
    // ================= FPS ===========================
    // =================================================

    uint32_t frame_count = 0;

    uint32_t fps = 0;

    TickType_t last_fps_tick =
        xTaskGetTickCount();

    TickType_t lastWakeTime =
        xTaskGetTickCount();

    // =================================================
    // ================= LOOP ==========================
    // =================================================

    while (1)
    {
        // =================================================
        // ================= CLEAR =========================
        // =================================================

        sprite.fillSprite(TFT_BLACK);

        // =================================================
        // ================= HORIZON DATA ==================
        // =================================================

        float pitch = g_imu_data.pitch;

        float roll = g_imu_data.roll;

        // =================================================
        // ================= SCREEN CENTER =================
        // =================================================

        int cx = 160;
        int cy = 120;

        int bank_radius = 90;

        // =================================================
        // ================= PITCH OFFSET ==================
        // =================================================

        int pitch_scale = 2;

        int pitch_offset =
            pitch * pitch_scale;

        // =================================================
        // ================= ROLL ==========================
        // =================================================

        float rad = roll * DEG_TO_RAD;

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
        // ================= HORIZON =======================
        // =================================================

        sprite.drawLine(
            x1,
            y1,
            x2,
            y2,
            TFT_WHITE
        );

        sprite.drawCircle(
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
                cx + cos(a) * (bank_radius - 10);

            int y1 =
                cy + sin(a) * (bank_radius - 10);

            int x2 =
                cx + cos(a) * bank_radius;

            int y2 =
                cy + sin(a) * bank_radius;

            sprite.drawLine(
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

        sprite.fillTriangle(
            px,
            py,

            px - 6,
            py - 10,

            px + 6,
            py - 10,

            TFT_YELLOW
        );

        // =================================================
        // ================= AIRCRAFT SYMBOL ===============
        // =================================================

        // skrzydła

        sprite.drawLine(
            cx - 25,
            cy,
            cx - 5,
            cy,
            TFT_YELLOW
        );

        sprite.drawLine(
            cx + 5,
            cy,
            cx + 25,
            cy,
            TFT_YELLOW
        );

        // środek

        sprite.fillCircle(
            cx,
            cy,
            3,
            TFT_YELLOW
        );

        // pionowy marker

        sprite.drawLine(
            cx,
            cy - 10,
            cx,
            cy - 4,
            TFT_YELLOW
        );

        for (int angle = -30; angle <= 30; angle += 10)
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

            // rotated points

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

            // draw rotated ladder

            sprite.drawLine(
                rx1,
                ry1,
                rx2,
                ry2,
                TFT_WHITE
            );

            sprite.setCursor(cx + 50, ly - 5);

            sprite.printf("%d", angle);
        }

        // =================================================
        // ================= CENTER MARK ===================
        // =================================================

        sprite.drawLine(
            cx - 15,
            cy,
            cx + 15,
            cy,
            TFT_YELLOW
        );

        sprite.drawLine(
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
            &sprite,
            10,
            10,
            "AHRS"
        );

        ui_draw_float(
            &sprite,
            10,
            40,
            "P",
            pitch
        );

        ui_draw_float(
            &sprite,
            10,
            65,
            "R",
            roll
        );

        // =================================================
        // ================= FPS ===========================
        // =================================================

        frame_count++;

        TickType_t now =
            xTaskGetTickCount();

        if ((now - last_fps_tick)
            >= pdMS_TO_TICKS(1000))
        {
            fps = frame_count;

            frame_count = 0;

            last_fps_tick = now;
        }

        ui_draw_float(
            &sprite,
            10,
            120,
            "FPS",
            (float)fps
        );

        ui_draw_float(
            &sprite,
            10,
            90,
            "G-R",
            g_imu_data.gyro_roll
        );

        ui_draw_float(
            &sprite,
            10,
            115,
            "G-P",
            g_imu_data.gyro_pitch
        );

        // =================================================
        // ================= PUSH ==========================
        // =================================================

        sprite.pushSprite(0, 0);

        // oddaj CPU schedulerowi
        taskYIELD();

        // =================================================
        // ================= 30 FPS ========================
        // =================================================

        vTaskDelayUntil(
            &lastWakeTime,
            pdMS_TO_TICKS(100)
        );
    }
}
