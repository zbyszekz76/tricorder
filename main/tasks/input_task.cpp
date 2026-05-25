// =================================================
// ================= input_task.cpp ================
// =================================================

#include "input_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "../game/space_game.h"
#include "../ui/ui_menu.h"
#include "../ui/ui_mode.h"
#include "../ui/ui_pages.h"

#include "esp_log.h"

// =================================================
// ===================== PINS ======================
// =================================================

// DOSTOSUJ DO SWOJEGO HARDWARE

#define ENCODER_A_PIN GPIO_NUM_6
#define ENCODER_B_PIN GPIO_NUM_7
#define ENCODER_BTN_PIN GPIO_NUM_18

// =================================================
// ===================== TAG =======================
// =================================================

static const char *TAG = "INPUT_TASK";

// =================================================
// ===================== TASK ======================
// =================================================

void input_task(void *arg)
{
    ESP_LOGI(TAG, "Input task started");

    // =================================================
    // ================= GPIO CONFIG ===================
    // =================================================

    gpio_set_direction(ENCODER_A_PIN, GPIO_MODE_INPUT);

    gpio_set_direction(ENCODER_B_PIN, GPIO_MODE_INPUT);

    gpio_set_direction(ENCODER_BTN_PIN, GPIO_MODE_INPUT);

    gpio_pullup_en(ENCODER_A_PIN);
    gpio_pullup_en(ENCODER_B_PIN);
    gpio_pullup_en(ENCODER_BTN_PIN);

    // =================================================
    // ================= STATES ========================
    // =================================================

    int last_a = gpio_get_level(ENCODER_A_PIN);

    int last_button = gpio_get_level(ENCODER_BTN_PIN);

    TickType_t button_press_time = 0;

    // =================================================
    // ================= LOOP ==========================
    // =================================================

    while(1)
    {
        // =================================================
        // ================= ENCODER =======================
        // =================================================

        int a = gpio_get_level(ENCODER_A_PIN);

        int b = gpio_get_level(ENCODER_B_PIN);

        // rising edge on A

        if((a == 1) && (last_a == 0))
        {
            // =============================================
            // ================= MENU MODE =================
            // =============================================

            if(get_current_page() == PAGE_MENU)
            {
                if(b == 0)
                {
                    next_tile();
                }
                else
                {
                    prev_tile();
                }
            }

            // =============================================
            // ================= PAGE MODE =================
            // =============================================

            else
            {
                // MANUAL MODE ONLY

                if(!ui_is_auto_mode())
                {
                    if(b == 0)
                    {
                        next_page();
                    }
                    else
                    {
                        prev_page();
                    }
                }
            }
        }

        last_a = a;

        // =================================================
        // ================= BUTTON ========================
        // =================================================

        int button = gpio_get_level(ENCODER_BTN_PIN);

        // =============================================
        // ================= BUTTON ====================
        // =============================================

        // =============================================
        // ============== BUTTON DOWN ==================
        // =============================================

        if((button == 0) && (last_button == 1))
        {
            button_press_time = xTaskGetTickCount();
        }

        // =============================================
        // =============== BUTTON UP ===================
        // =============================================

        if((button == 1) && (last_button == 0))
        {
            TickType_t press_time = xTaskGetTickCount() - button_press_time;

            // =========================================
            // ============== SHORT PRESS ==============
            // =========================================

            if(press_time < pdMS_TO_TICKS(800))
            {
                // =====================================
                // =============== MENU ================
                // =====================================

                if(get_current_page() == PAGE_MENU)
                {
                    switch(get_selected_tile())
                    {
                    case 0:

                        set_current_page(PAGE_AHRS);

                        break;

                    case 1:

                        set_current_page(PAGE_CLOCK);

                        break;

                    case 2:

                        set_current_page(PAGE_SYSTEM);

                        break;

                    case 3:

                        set_current_page(PAGE_IMU_DEBUG);

                        break;

                    case 4:

                        set_current_page(PAGE_GAME);

                        break;

                    default:
                        break;
                    }
                }

                // =====================================
                // ============== NORMAL ===============
                // =====================================

                else if(get_current_page() == PAGE_GAME)
                {
                    if(isGameMenu())
                    {
                        startGame();
                    }
                    else if(isGamePlaying())
                    {
                        fireBullet();
                    }
                }
                else
                {
                    ui_toggle_mode();
                }
            }

            // =========================================
            // =============== LONG PRESS ==============
            // =========================================

            else
            {
                set_current_page(PAGE_MENU);
            }
        }

        last_button = button;

        // =================================================
        // ================= DELAY =========================
        // =================================================

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}