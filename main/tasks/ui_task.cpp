#include "ui_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "../drivers/lgfx_display.h"

#include "../common/shared_data.h"

#include "../ui/ui_helpers.h"

// =================================================
// ===================== TAG =======================
// =================================================

static const char *TAG = "UI_TASK";

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

    uint32_t fps = 0;

    uint32_t last_fps_time = 0;

    while (1)
    {
        // ================= CLEAR =================

        sprite.fillSprite(TFT_BLACK);

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
        50,
        "AX",
        g_imu_data.ax
    );

    ui_draw_float(
        &sprite,
        10,
        80,
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

        // ================= FPS =================

        fps++;

        if (xTaskGetTickCount() - last_fps_time > 1000 / portTICK_PERIOD_MS)
        {
            last_fps_time = xTaskGetTickCount();

            sprite.setCursor(10, 180);

            sprite.printf("FPS: %lu", fps);

            fps = 0;
        }

        // ================= PUSH =================

        sprite.pushSprite(0, 0);

        vTaskDelay(pdMS_TO_TICKS(16));
    }
}