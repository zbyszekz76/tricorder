#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "drivers/fxos8700.h"
#include "drivers/fxas21002.h"
#include "tasks/imu_task.h"

#include "network/wifi_manager.h"

#include "tasks/input_task.h"


// #include "drivers/ili9341_display.h"
#include "tasks/ui_task.h"

// =================================================
// ===================== TAG =======================
// =================================================

static const char *TAG = "MAIN";

// wifi_manager_init();

// =================================================
// ===================== MAIN ======================
// =================================================

void app_main(void)
{
    wifi_manager_init();

    ESP_LOGI(TAG, "System start");

    // ================= DISPLAY =================

    // display_init();

    // ================= IMU =================

    if (fxos8700_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "IMU init failed");
    }

    if (fxas21002_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "GYRO init failed");
    }
    else
    {
        ESP_LOGI(TAG, "GYRO online");
    }

    // ================= IMU TASK =================

    xTaskCreate(
        imu_task,
        "imu_task",
        8192,
        NULL,
        5,
        NULL
    );

    // ================= UI TASK =================

    xTaskCreatePinnedToCore(
        ui_task,
        "ui_task",
        8192,
        NULL,
        1,
        NULL,
        1
    );

    xTaskCreatePinnedToCore(
        input_task,
        "input_task",
        4096,
        NULL,
        1,
        NULL,
        0
    );

    ESP_LOGI(TAG, "System ready");
}