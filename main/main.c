#include <stdio.h>
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// ===================== TAG =====================
static const char *TAG = "IMU";

// ===================== I2C =====================
#define I2C_SDA_PIN 8
#define I2C_SCL_PIN 9

#define I2C_PORT 0
#define I2C_FREQ_HZ 400000

// ===================== FXOS8700 =====================
#define FXOS8700_ADDR 0x1E
#define FXOS8700_WHO_AM_I 0x0D

// powinno zwrócić 0xC7
#define FXOS8700_ID 0xC7

// uchwyt magistrali
i2c_master_bus_handle_t bus_handle;

// uchwyt urządzenia
i2c_master_dev_handle_t imu_handle;


// =================================================
// ===================== I2C INIT ===================
// =================================================

void init_i2c(void)
{
    // konfiguracja magistrali
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = I2C_SCL_PIN,
        .sda_io_num = I2C_SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true
    };

    // utworzenie magistrali
    ESP_ERROR_CHECK(
        i2c_new_master_bus(&bus_config, &bus_handle)
    );

    // konfiguracja urządzenia
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = FXOS8700_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };

    // dodanie urządzenia do magistrali
    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(
            bus_handle,
            &dev_config,
            &imu_handle
        )
    );

    ESP_LOGI(TAG, "I2C initialized");
}


// =================================================
// ===================== READ REG ===================
// =================================================

esp_err_t read_register(
    uint8_t reg,
    uint8_t *data,
    size_t len
)
{
    return i2c_master_transmit_receive(
        imu_handle,
        &reg,
        1,
        data,
        len,
        100
    );
}


// =================================================
// ===================== TASK =======================
// =================================================

void imu_task(void *arg)
{
    uint8_t who_am_i = 0;

    while (1)
    {
        esp_err_t ret = read_register(
            FXOS8700_WHO_AM_I,
            &who_am_i,
            1
        );

        if (ret == ESP_OK)
        {
            if (who_am_i == FXOS8700_ID)
            {
                ESP_LOGI(
                    TAG,
                    "FXOS8700 detected: 0x%02X",
                    who_am_i
                );
            }
            else
            {
                ESP_LOGW(
                    TAG,
                    "Unexpected ID: 0x%02X",
                    who_am_i
                );
            }
        }
        else
        {
            ESP_LOGE(TAG, "I2C read failed");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


// =================================================
// ===================== MAIN =======================
// =================================================

void app_main(void)
{
    ESP_LOGI(TAG, "System start");

    init_i2c();

    xTaskCreate(
        imu_task,
        "imu_task",
        4096,
        NULL,
        5,
        NULL
    );
}