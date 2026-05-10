#include "imu_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "../drivers/fxos8700.h"
#include "../common/shared_data.h"

// =================================================
// ===================== TAG =======================
// =================================================

static const char *TAG = "IMU_TASK";

// =================================================
// ===================== TASK ======================
// =================================================

void imu_task(void *arg)
{
    ESP_LOGI(TAG, "IMU task started");

    while (1)
    {
        float ax, ay, az;

        if (fxos8700_read_accel(
                &ax,
                &ay,
                &az
            ) == ESP_OK)
        {
            // shared data update
            g_imu_data.ax = ax;
            g_imu_data.ay = ay;
            g_imu_data.az = az;

            // debug
            static uint32_t counter = 0;

            counter++;

            if (counter >= 10)
            {
                ESP_LOGI(
                    TAG,
                    "AX: %.2f AY: %.2f AZ: %.2f",
                    ax,
                    ay,
                    az
                );

                counter = 0;
            }
        }
        else
        {
            ESP_LOGE(TAG, "Read error");
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}