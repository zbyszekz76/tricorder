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

    float gyro_roll = 0.0f;
    float gyro_pitch = 0.0f;

    float fused_roll = 0.0f;
    float fused_pitch = 0.0f;

    TickType_t last_tick =
        xTaskGetTickCount();

    while (1)
    {
        TickType_t now =
            xTaskGetTickCount();

        float dt =
            (now - last_tick)
            * portTICK_PERIOD_MS
            / 1000.0f;

        last_tick = now;

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

        fused_roll =
            0.98f *
            (fused_roll + gx * dt)
            +
            0.02f *
            filtered_roll;

        fused_pitch =
            0.98f *
            (fused_pitch + gy * dt)
            +
            0.02f *
            filtered_pitch;

        // shared data

        g_imu_data.roll  = fused_roll;

        g_imu_data.pitch = fused_pitch;           

            // debug
            static uint32_t acc_counter = 0;

            acc_counter++;

            if (acc_counter >= 10)
            {
                ESP_LOGI(
                    TAG,
                    "AX: %.2f AY: %.2f AZ: %.2f",
                    ax,
                    ay,
                    az
                );

                acc_counter = 0;
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

            gyro_roll += gx * dt;
            gyro_pitch += gy * dt;

            g_imu_data.gyro_roll = gyro_roll;
            g_imu_data.gyro_pitch = gyro_pitch;

            static uint32_t gyro_counter = 0;

            gyro_counter++;

            if (gyro_counter >= 20)
            {
                ESP_LOGI(
                    TAG,
                    "GX: %.2f GY: %.2f GZ: %.2f",
                    gx,
                    gy,
                    gz
                );

                gyro_counter = 0;
            }
        }
        else
        {
            ESP_LOGE(TAG, "Gyro read error");
        }
        

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
