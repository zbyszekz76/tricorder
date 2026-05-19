// =================================================
// ================= ui_task.cpp ===================
// =================================================

#include "ui_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "../drivers/lgfx_display.h"

#include "../common/shared_data.h"

#include "../ui/ui_renderer.h"

#include "../ui/ui_pages.h"

#include "../ui/page_menu.h"

#include "../ui/ui_mode.h"

#include "../network/wifi_manager.h"

#include "../game/space_game.h"

// =================================================
// ===================== TAG =======================
// =================================================

static const char *TAG = "UI_TASK";

static TickType_t last_time_update = 0;

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

    // =================================================
    // ================= TFT INIT ======================
    // =================================================

    tft.init();

    tft.setRotation(3);

    // =================================================
    // ================= SPRITE ========================
    // =================================================

    sprite.createSprite(320, 240);

    sprite.setTextSize(2);

    // =================================================
    // ================= FPS ===========================
    // =================================================

    uint32_t frame_count = 0;

    uint32_t fps = 0;

    TickType_t last_fps_tick =
        xTaskGetTickCount();

    TickType_t lastWakeTime =
        xTaskGetTickCount();

    TickType_t last_page_change =
    xTaskGetTickCount();

    // =================================================
    // ================= LOOP ==========================
    // =================================================

     
    while (1)
    {
        TickType_t now = xTaskGetTickCount();

        if ((now - last_time_update)
            >= pdMS_TO_TICKS(1000))
        {
            wifi_update_time();

            last_time_update = now;
        }

        // =================================================
        // ================= CLEAR =========================
        // =================================================

        sprite.fillSprite(TFT_BLACK);

        // =================================================
// ================= UI MODE =======================
// =================================================

        sprite.setTextColor(TFT_CYAN);

        sprite.setTextSize(1);

        sprite.setCursor(270, 10);

// =================================================
// ================= AUTO ROTATE ===================
// =================================================

        if(
            ui_is_auto_mode()
            &&
            get_current_page() != PAGE_MENU
        )
        {
            if((now - last_page_change)
                >= pdMS_TO_TICKS(5000))
            {
                next_page();

                // zabezpieczenie:
                // nigdy nie wracaj automatycznie do MENU

                if(get_current_page() == PAGE_MENU)
                {
                    next_page();

                    
                }
                last_page_change = now;
                
            }
        }

        // =================================================
        // ================= DATA ==========================
        // =================================================

        float pitch =
            g_imu_data.pitch;

        float roll =
            g_imu_data.roll;

        // =================================================
        // ================= CENTER ========================
        // =================================================

        int cx = 160;
        int cy = 120;

        // =================================================
        // ================= PITCH =========================
        // =================================================

        int pitch_scale = 2;

        int pitch_offset =
            pitch * pitch_scale;

        // =================================================
        // ================= RENDER ========================
        // =================================================

        static constexpr float DEG_TO_RAD =
            0.017453292519943295769f;

        float rad =
            roll * DEG_TO_RAD;

        int horizon_len = 260;

        int x1 =
            cx - horizon_len * cos(rad);

        int y1 =
            cy - horizon_len * sin(rad)
            + pitch_offset;

        int x2 =
            cx + horizon_len * cos(rad);

        int y2 =
            cy + horizon_len * sin(rad)
            + pitch_offset;

            frame_count++;

if ((now - last_fps_tick)
    >= pdMS_TO_TICKS(1000))
{
    fps = frame_count;

    frame_count = 0;

    last_fps_tick = now;
}
        
switch(get_current_page())
{
    // =============================================
    // ================ AHRS =======================
    // =============================================

    case PAGE_MENU:

    draw_menu_page(&sprite);

    break;
    
    case PAGE_AHRS:

        draw_sky_ground(
            &sprite,
            x1,
            y1,
            x2,
            y2
        );

        draw_horizon(
            &sprite,
            cx,
            cy,
            roll,
            pitch_offset
        );

        draw_bank_scale(
            &sprite,
            cx,
            cy,
            roll
        );

        draw_pitch_ladder(
            &sprite,
            cx,
            cy,
            roll,
            pitch_offset
        );

        draw_aircraft_symbol(
            &sprite,
            cx,
            cy
        );

        draw_overlay(
            &sprite,
            pitch,
            roll,
            fps,
            g_imu_data.gyro_roll,
            g_imu_data.gyro_pitch
        );

        break;

        case PAGE_CLOCK:

            // =========================================
            // ================= TIME ==================
            // =========================================

            sprite.setTextColor(TFT_CYAN);

            sprite.setTextSize(5);

            sprite.setCursor(45, 70);

            sprite.printf(
                "%s",
                g_imu_data.current_time
            );

            // =========================================
            // ================= DATE ==================
            // =========================================

            sprite.setTextColor(TFT_WHITE);

            sprite.setTextSize(2);

            sprite.setCursor(85, 145);

            sprite.printf(
                "%s",
                g_imu_data.current_date
            );

            // =========================================
            // ================= WIFI ==================
            // =========================================

            sprite.setTextSize(1);

            sprite.setCursor(20, 210);

            sprite.printf(
                "IP: %s",
                g_imu_data.ip_address
            );

            break;

    // =============================================
    // ================ IMU DEBUG ==================
    // =============================================

    case PAGE_IMU_DEBUG:

        sprite.setCursor(20, 20);

        sprite.printf(
            "AX: %.2f",
            g_imu_data.ax
        );

        sprite.setCursor(20, 50);

        sprite.printf(
            "AY: %.2f",
            g_imu_data.ay
        );

        sprite.setCursor(20, 80);

        sprite.printf(
            "AZ: %.2f",
            g_imu_data.az
        );

        sprite.setCursor(20, 120);

        sprite.printf(
            "GX: %.2f",
            g_imu_data.gx
        );

        sprite.setCursor(20, 150);

        sprite.printf(
            "GY: %.2f",
            g_imu_data.gy
        );

        sprite.setCursor(20, 180);

        sprite.printf(
            "GZ: %.2f",
            g_imu_data.gz
        );

        break;


    // =============================================
    // ================GAME =====================
    // =============================================
        case PAGE_GAME:

            drawSpaceGame(&sprite);

            break;

    // =============================================
    // ================ SYSTEM =====================
    // =============================================

    case PAGE_SYSTEM:

        sprite.setCursor(20, 20);

        sprite.printf(
            "TIME: %s",
            g_imu_data.current_time
        );

        sprite.setCursor(20, 50);

        sprite.printf(
            "DATE: %s",
            g_imu_data.current_date
        );

        sprite.setCursor(20, 80);

        sprite.printf(
            "IP: %s",
            g_imu_data.ip_address
        );

        sprite.setCursor(20, 110);

        sprite.printf(
            "RSSI: %d dBm",
            g_imu_data.wifi_rssi
        );

        sprite.setCursor(20, 140);

        sprite.printf(
            "WIFI: %s",
            g_imu_data.wifi_connected
                ? "CONNECTED"
                : "DISCONNECTED"
        );

        break;

        default:
            break;
}



        // =================================================
        // ================= PUSH ==========================
        // =================================================

        sprite.pushSprite(0, 0);

        // =================================================
        // ================= YIELD =========================
        // =================================================

        taskYIELD();

        // =================================================
        // ================= FPS LIMIT =====================
        // =================================================

        vTaskDelayUntil(
            &lastWakeTime,
            pdMS_TO_TICKS(150)
        );
    }
}