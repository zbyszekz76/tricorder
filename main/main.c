#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_heap_caps.h"

#include "driver/spi_master.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

#include "esp_lcd_ili9341.h"

// =================================================
// ===================== TAG =======================
// =================================================

static const char *TAG = "TFT";

// =================================================
// ===================== TFT PINS ==================
// =================================================

// #define PIN_NUM_MOSI 38
// #define PIN_NUM_MISO 47
// #define PIN_NUM_CLK  48

// #define PIN_NUM_CS   21
// #define PIN_NUM_DC   18
// #define PIN_NUM_RST  17

#define PIN_NUM_MOSI 11
#define PIN_NUM_MISO 13
#define PIN_NUM_CLK  12

#define PIN_NUM_CS   10
#define PIN_NUM_DC   14
#define PIN_NUM_RST  15

// =================================================
// ===================== LCD =======================
// =================================================

#define LCD_HOST SPI2_HOST

#define LCD_H_RES 240
#define LCD_V_RES 320

// =================================================
// ===================== MAIN ======================
// =================================================

void app_main(void)
{
    ESP_LOGI(TAG, "ILI9341 TEST START");

    // =================================================
    // ===================== SPI BUS ===================
    // =================================================

    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_CLK,
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t),
    };

    ESP_ERROR_CHECK(
        spi_bus_initialize(
            LCD_HOST,
            &buscfg,
            SPI_DMA_CH_AUTO
        )
    );

    ESP_LOGI(TAG, "SPI bus initialized");

    // =================================================
    // ===================== PANEL IO ==================
    // =================================================

    esp_lcd_panel_io_handle_t io_handle = NULL;

    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_DC,
        .cs_gpio_num = PIN_NUM_CS,

        .pclk_hz = 40000000,

        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,

        .spi_mode = 0,

        .trans_queue_depth = 10,
    };

    ESP_ERROR_CHECK(
        esp_lcd_new_panel_io_spi(
            (esp_lcd_spi_bus_handle_t)LCD_HOST,
            &io_config,
            &io_handle
        )
    );

    ESP_LOGI(TAG, "Panel IO initialized");

    // =================================================
    // ===================== PANEL =====================
    // =================================================

    esp_lcd_panel_handle_t panel_handle = NULL;

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };

    ESP_ERROR_CHECK(
        esp_lcd_new_panel_ili9341(
            io_handle,
            &panel_config,
            &panel_handle
        )
    );

    ESP_LOGI(TAG, "ILI9341 panel created");

    // =================================================
    // ===================== RESET =====================
    // =================================================

    ESP_ERROR_CHECK(
        esp_lcd_panel_reset(panel_handle)
    );

    ESP_ERROR_CHECK(
        esp_lcd_panel_init(panel_handle)
    );

    ESP_ERROR_CHECK(
        esp_lcd_panel_disp_on_off(panel_handle, true)
    );

    ESP_LOGI(TAG, "Display initialized");

    // =================================================
    // ===================== BUFFER ====================
    // =================================================

    uint16_t *buffer = heap_caps_malloc(
        LCD_H_RES * LCD_V_RES * sizeof(uint16_t),
        MALLOC_CAP_DMA
    );

    if (buffer == NULL)
    {
        ESP_LOGE(TAG, "Buffer allocation failed");
        return;
    }

    // =================================================
    // ===================== LOOP ======================
    // =================================================

    while (1)
    {
        // RED
        for (int i = 0; i < LCD_H_RES * LCD_V_RES; i++)
        {
            buffer[i] = 0xF800;
        }

        esp_lcd_panel_draw_bitmap(
            panel_handle,
            0,
            0,
            LCD_H_RES,
            LCD_V_RES,
            buffer
        );

        ESP_LOGI(TAG, "RED");

        vTaskDelay(pdMS_TO_TICKS(1000));

        // GREEN
        for (int i = 0; i < LCD_H_RES * LCD_V_RES; i++)
        {
            buffer[i] = 0x07E0;
        }

        esp_lcd_panel_draw_bitmap(
            panel_handle,
            0,
            0,
            LCD_H_RES,
            LCD_V_RES,
            buffer
        );

        ESP_LOGI(TAG, "GREEN");

        vTaskDelay(pdMS_TO_TICKS(1000));

        // BLUE
        for (int i = 0; i < LCD_H_RES * LCD_V_RES; i++)
        {
            buffer[i] = 0x001F;
        }

        esp_lcd_panel_draw_bitmap(
            panel_handle,
            0,
            0,
            LCD_H_RES,
            LCD_V_RES,
            buffer
        );

        ESP_LOGI(TAG, "BLUE");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
