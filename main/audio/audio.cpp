#include "audio.h"

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
// ================== AUDIO =======================
// =================================================

static int sound_timer = 0;

// =================================================
// ================== TONES =======================
// =================================================

#define NOTE_A4 440
#define NOTE_F4 349
#define NOTE_C5 523
#define NOTE_E5 659
#define NOTE_G5 784

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
// ==================== INIT =======================
// =================================================

void audio_init()
{
    gpio_set_direction(
        AUDIO_PIN,
        GPIO_MODE_OUTPUT
    );

    gpio_set_level(
        AUDIO_PIN,
        0
    );
}

// =================================================
// =================== UPDATE ======================
// =================================================

void audio_update()
{
    if(sound_timer > 0)
    {
        sound_timer--;
    }
    else
    {
        gpio_set_level(
            AUDIO_PIN,
            0
        );
    }
}

// =================================================
// ================= LASER SOUND ===================
// =================================================

void playLaserSound()
{
    playTone(
        1800,
        40
    );
}

// =================================================
// =============== EXPLOSION SOUND =================
// =================================================

void playExplosionSound()
{
    playTone(
        250,
        120
    );
}

// =================================================
// ================= INTRO MUSIC ===================
// =================================================

void playIntroMusic()
{
    // =========================================
    // ============ IMPERIAL MARCH =============
    // =========================================

    // A A A

    playTone(440, 500);
    esp_rom_delay_us(50000);

    playTone(440, 500);
    esp_rom_delay_us(50000);

    playTone(440, 500);
    esp_rom_delay_us(80000);

    // F C A

    playTone(349, 350);
    esp_rom_delay_us(20000);

    playTone(523, 150);
    esp_rom_delay_us(20000);

    playTone(440, 650);
    esp_rom_delay_us(120000);

    // E E E

    playTone(659, 500);
    esp_rom_delay_us(50000);

    playTone(659, 500);
    esp_rom_delay_us(50000);

    playTone(659, 500);
    esp_rom_delay_us(80000);

    // F C G#

    playTone(349, 350);
    esp_rom_delay_us(20000);

    playTone(523, 150);
    esp_rom_delay_us(20000);

    playTone(415, 650);
    esp_rom_delay_us(150000);

    // A A A

    playTone(440, 500);
    esp_rom_delay_us(50000);

    playTone(440, 500);
    esp_rom_delay_us(50000);

    playTone(440, 500);
    esp_rom_delay_us(80000);

    // A C E

    playTone(440, 350);
    esp_rom_delay_us(20000);

    playTone(523, 150);
    esp_rom_delay_us(20000);

    playTone(659, 650);
    esp_rom_delay_us(100000);

    // F C A

    playTone(349, 350);
    esp_rom_delay_us(20000);

    playTone(523, 150);
    esp_rom_delay_us(20000);

    playTone(440, 900);

    // =========================================
    // ================= STOP ===================
    // =========================================

    gpio_set_level(
        AUDIO_PIN,
        0
    );
}

// void playIntroMusic()
// {
//     // =========================================
//     // ============ IMPERIAL MARCH =============
//     // =========================================

//     playTone(659, 600);   // E
//     vTaskDelay(pdMS_TO_TICKS(50));

//     playTone(659, 600);   // E
//     vTaskDelay(pdMS_TO_TICKS(50));

//     playTone(659, 600);   // E
//     vTaskDelay(pdMS_TO_TICKS(80));

//     playTone(523, 300);   // C
//     vTaskDelay(pdMS_TO_TICKS(20));

//     vTaskDelay(pdMS_TO_TICKS(120)); // pause

//     playTone(392, 600);   // G
//     vTaskDelay(pdMS_TO_TICKS(50));

//     playTone(659, 600);   // E
//     vTaskDelay(pdMS_TO_TICKS(80));

//     playTone(523, 300);   // C
//     vTaskDelay(pdMS_TO_TICKS(20));

//     vTaskDelay(pdMS_TO_TICKS(120)); // pause

//     playTone(392, 600);   // G
//     vTaskDelay(pdMS_TO_TICKS(50));

//     playTone(659, 800);   // E
//     vTaskDelay(pdMS_TO_TICKS(150));

//     // =========================================
//     // ============== SECOND PART ===============
//     // =========================================

//     playTone(988, 600);   // B5
//     vTaskDelay(pdMS_TO_TICKS(40));

//     playTone(988, 600);
//     vTaskDelay(pdMS_TO_TICKS(40));

//     playTone(988, 600);
//     vTaskDelay(pdMS_TO_TICKS(80));

//     playTone(1047, 300);  // C6
//     vTaskDelay(pdMS_TO_TICKS(30));

//     vTaskDelay(pdMS_TO_TICKS(120));

//     playTone(392, 600);   // G
//     vTaskDelay(pdMS_TO_TICKS(40));

//     playTone(622, 700);   // D#
//     vTaskDelay(pdMS_TO_TICKS(40));

//     playTone(523, 300);   // C
//     vTaskDelay(pdMS_TO_TICKS(20));

//     vTaskDelay(pdMS_TO_TICKS(120));

//     playTone(392, 600);   // G
//     vTaskDelay(pdMS_TO_TICKS(40));

//     playTone(659, 800);   // E
//     vTaskDelay(pdMS_TO_TICKS(150));

//     // =========================================
//     // ================= STOP ===================
//     // =========================================

//     gpio_set_level(
//         AUDIO_PIN,
//         0
//     );
// }