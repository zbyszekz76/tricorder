#include "rttl_task.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_rom_sys.h"

// =================================================
// ================= CONFIG =======================
// =================================================

static const gpio_num_t AUDIO_PIN =
    GPIO_NUM_17;

// =================================================
// ================= NOTES ========================
// =================================================

#define a4 440
#define f4 349
#define c5 523
#define e5 659
#define g5 784

// =================================================
// ================= PLAY TONE ====================
// =================================================

static void playTone(
    int freq,
    int duration_ms
)
{
    int delay_us =
        1000000 / (freq * 2);

    int cycles =
        (freq * duration_ms) / 1000;

    for(int i = 0; i < cycles; i++)
    {
        gpio_set_level(
            AUDIO_PIN,
            1
        );

        esp_rom_delay_us(delay_us);

        gpio_set_level(
            AUDIO_PIN,
            0
        );

        esp_rom_delay_us(delay_us);
    }
}

// =================================================
// ================= RTTL TASK ====================
// =================================================

static void imperialTask(void *pv)
{
    gpio_set_direction(
        AUDIO_PIN,
        GPIO_MODE_OUTPUT
    );

    // =============================================
    // ================= MELODY ====================
    // =============================================

    playTone(a4, 500);
    vTaskDelay(pdMS_TO_TICKS(50));

    playTone(a4, 500);
    vTaskDelay(pdMS_TO_TICKS(50));

    playTone(a4, 500);
    vTaskDelay(pdMS_TO_TICKS(50));

    playTone(f4, 350);
    playTone(c5, 150);

    playTone(a4, 650);

    // =============================================
    // ================= STOP ======================
    // =============================================

    gpio_set_level(
        AUDIO_PIN,
        0
    );

    vTaskDelete(NULL);
}


// =================================================
// ============== START MUSIC TASK ================
// =================================================

void playImperialMarch()
{
    xTaskCreatePinnedToCore(
        imperialTask,
        "imperial_task",
        4096,
        NULL,
        1,
        NULL,
        1
    );
}