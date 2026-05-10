#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "drivers/fxos8700.h"
#include "tasks/imu_task.h"

#include "drivers/ili9341_display.h"
#include "tasks/ui_task.h"

// =================================================
// ===================== TAG =======================
// =================================================

static const char *TAG = "MAIN";

// =================================================
// ===================== MAIN ======================
// =================================================

void app_main(void)
{
    ESP_LOGI(TAG, "System start");

    // ================= DISPLAY =================

    display_init();

    // ================= IMU =================

    if (fxos8700_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "IMU init failed");
    }
    else
    {
        xTaskCreate(
            imu_task,
            "imu_task",
            8192,
            NULL,
            5,
            NULL
        );
    }

    // ================= UI TASK =================

    xTaskCreate(
        ui_task,
        "ui_task",
        8192,
        NULL,
        4,
        NULL
    );

    ESP_LOGI(TAG, "System ready");
}