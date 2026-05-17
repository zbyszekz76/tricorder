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

    // =================================================
    // ================= GYRO CALIBRATION =============
    // =================================================

    float gyro_bias_x = 0.0f;
    float gyro_bias_y = 0.0f;
    float gyro_bias_z = 0.0f;

    ESP_LOGI(TAG, "Gyro calibration... keep device ABSOLUTELY still for 10 seconds");

    // 10 seconds * 100 Hz = 1000 samples
    const int calibration_samples = 1000;

    for (int i = 0; i < calibration_samples; i++)
    {
        float cgx, cgy, cgz;

        if (fxas21002_read_gyro(
                &cgx,
                &cgy,
                &cgz
            ) == ESP_OK)
        {
            gyro_bias_x += cgx;
            gyro_bias_y += cgy;
            gyro_bias_z += cgz;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    gyro_bias_x /= calibration_samples;
    gyro_bias_y /= calibration_samples;
    gyro_bias_z /= calibration_samples;

    ESP_LOGI(
        TAG,
        "Gyro bias FINAL: X=%.4f Y=%.4f Z=%.4f",
        gyro_bias_x,
        gyro_bias_y,
        gyro_bias_z
    );
    
    // Verify bias values are reasonable
    float bias_magnitude = sqrtf(
        gyro_bias_x * gyro_bias_x +
        gyro_bias_y * gyro_bias_y +
        gyro_bias_z * gyro_bias_z
    );
    
    if (bias_magnitude > 5.0f)
    {
        ESP_LOGW(TAG, "Warning: Bias magnitude %.2f exceeds 5 dps - recalibrate!", bias_magnitude);
    }

    // =================================================
    // ================= FILTERS =======================
    // =================================================

    float filtered_pitch = 0.0f;
    float filtered_roll  = 0.0f;

    // complementary filter

    float fused_roll = 0.0f;
    float fused_pitch = 0.0f;

    // LPF coefficient

    const float alpha = 0.15f;

    // gyro integration

    float gyro_roll = 0.0f;
    float gyro_pitch = 0.0f;

    TickType_t last_tick =
        xTaskGetTickCount();

    // =================================================
    // ================= MAIN LOOP =====================
    // =================================================

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

                // =================================================
        // ================= GYRO ===========================
        // =================================================

        if (fxas21002_read_gyro(
                &gx,
                &gy,
                &gz
            ) == ESP_OK)
        {
            // RAW values before bias correction
            float gx_raw = gx;
            float gy_raw = gy;
            float gz_raw = gz;
            
            // remove bias
            gx -= gyro_bias_x;
            gy -= gyro_bias_y;
            gz -= gyro_bias_z;

            // shared gyro

            g_imu_data.gx = gx;
            g_imu_data.gy = gy;
            g_imu_data.gz = gz;

            // integration

            gyro_roll += gx * dt;
            gyro_pitch += gy * dt;

            g_imu_data.gyro_roll =
                gyro_roll;

            g_imu_data.gyro_pitch =
                gyro_pitch;

            // debug

            static uint32_t gyro_counter = 0;

            gyro_counter++;

            if (gyro_counter >= 100)
            {
                ESP_LOGI(
                    TAG,
                    "GX: %.3f (raw: %.3f) GY: %.3f (raw: %.3f) GZ: %.3f (raw: %.3f)",
                    gx,
                    gx_raw,
                    gy,
                    gy_raw,
                    gz,
                    gz_raw
                );

                gyro_counter = 0;
            }
        }
        else
        {
            ESP_LOGE(TAG, "Gyro read error");
        }

        // =================================================
        // ================= ACCEL =========================
        // =================================================

        if (fxos8700_read_accel(
                &ax,
                &ay,
                &az
            ) == ESP_OK)
        {
            // shared raw accel

            g_imu_data.ax = ax;
            g_imu_data.ay = ay;
            g_imu_data.az = az;

            // =================================================
            // ================= ANGLES ========================
            // =================================================

            // roll = left/right tilt

            float raw_roll =
                atan2f(ax, az)
                * 57.2958f;

            // pitch = front/back tilt

            float raw_pitch =
                atan2f(
                    -ay,
                    sqrtf(ax * ax + az * az)
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
            // ================= COMPLEMENTARY FILTER ==========
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

            // =================================================
            // ================= SHARED DATA ===================
            // =================================================

            g_imu_data.roll  = fused_roll;
            g_imu_data.pitch = fused_pitch;

            // =================================================
            // ================= DEBUG =========================
            // =================================================

            static uint32_t acc_counter = 0;

            acc_counter++;

            if (acc_counter >= 100)
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
            ESP_LOGE(TAG, "Accel read error");
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}