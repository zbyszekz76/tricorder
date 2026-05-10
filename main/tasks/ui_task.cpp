#include "ui_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "../drivers/lgfx_display.h"

#include "../common/shared_data.h"

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

        sprite.setCursor(10, 10);

        sprite.println("TRICORDER CORE");

        // ================= IMU =================

        sprite.setCursor(10, 50);

        sprite.printf(
            "AX: %.2f\n",
            g_imu_data.ax
        );
        sprite.setCursor(10, 70);
        sprite.printf(
            "AY: %.2f\n",
            g_imu_data.ay
        );
        sprite.setCursor(10, 90);
        sprite.printf(
            "AZ: %.2f\n",
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