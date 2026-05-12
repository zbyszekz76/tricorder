#include "imu_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include <math.h>

#include "../drivers/fxos8700.h"
#include "../drivers/fxas21002.h"
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

    float filtered_pitch = 0.0f;
    float filtered_roll  = 0.0f;

// współczynnik filtru
    const float alpha = 0.15f;

    while (1)
    {
        float ax, ay, az;
        float gx, gy, gz;

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

        float raw_roll =
            atan2f(ay, az) * 57.2958f;

        float raw_pitch =
            atan2f(
                -ax,
                sqrtf(ay * ay + az * az)
            ) * 57.2958f;

        // =================================================
        // ================= LOW PASS FILTER ===============
        // =================================================

        filtered_roll =
            alpha * raw_roll +
            (1.0f - alpha) * filtered_roll;

        filtered_pitch =
            alpha * raw_pitch +
            (1.0f - alpha) * filtered_pitch;

        // =================================================
        // ================= SHARED DATA ===================
        // =================================================

        g_imu_data.roll  = filtered_roll;
        g_imu_data.pitch = filtered_pitch;           

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

        if (fxas21002_read_gyro(
                &gx,
                &gy,
                &gz
            ) == ESP_OK)
        {
            g_imu_data.gx = gx;
            g_imu_data.gy = gy;
            g_imu_data.gz = gz;

            static uint32_t counter = 0;

            counter++;

            if (counter >= 20)
            {
                ESP_LOGI(
                    TAG,
                    "GX: %.2f GY: %.2f GZ: %.2f",
                    gx,
                    gy,
                    gz
                );

                counter = 0;
            }
        }
        else
        {
            ESP_LOGE(TAG, "Gyro read error");
        }
        

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
