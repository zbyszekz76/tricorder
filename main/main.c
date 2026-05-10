#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "drivers/fxos8700.h"

#include "tasks/imu_task.h"

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

    // init IMU
    ESP_ERROR_CHECK(
        fxos8700_init()
    );

    // create task
    xTaskCreate(
        imu_task,
        "imu_task",
        4096,
        NULL,
        5,
        NULL
    );

    ESP_LOGI(TAG, "System ready");
}