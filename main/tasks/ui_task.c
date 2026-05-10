#include "ui_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "../drivers/ili9341_display.h"

// =================================================
// ===================== TAG =======================
// =================================================

static const char *TAG = "UI_TASK";

// =================================================
// ===================== TASK ======================
// =================================================

void ui_task(void *arg)
{
    ESP_LOGI(TAG, "UI task started");

    while (1)
    {
        display_fill(0xF800);

        ESP_LOGI(TAG, "RED");

        vTaskDelay(pdMS_TO_TICKS(1000));

        display_fill(0x07E0);

        ESP_LOGI(TAG, "GREEN");

        vTaskDelay(pdMS_TO_TICKS(1000));

        display_fill(0x001F);

        ESP_LOGI(TAG, "BLUE");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}