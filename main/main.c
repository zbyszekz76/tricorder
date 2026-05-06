#include <stdio.h>
#include <math.h>
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
#define FXOS8700_CTRL_REG1 0x2A
#define FXOS8700_XYZ_DATA_CFG 0x0E
#define FXOS8700_OUT_X_MSB 0x01

#define FXOS8700_ID 0xC7

// uchwyty
i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t imu_handle;

// =================================================
// ===================== I2C INIT ===================
// =================================================

void init_i2c(void)
{
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = I2C_SCL_PIN,
        .sda_io_num = I2C_SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = FXOS8700_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &imu_handle));

    ESP_LOGI(TAG, "I2C initialized");
}

// =================================================
// ===================== WRITE ======================
// =================================================

void write_reg(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};

    ESP_ERROR_CHECK(
        i2c_master_transmit(
            imu_handle,
            data,
            2,
            100
        )
    );
}

// =================================================
// ===================== READ =======================
// =================================================

esp_err_t read_reg(uint8_t reg, uint8_t *data, size_t len)
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
// ===================== INIT ACC ===================
// =================================================

void init_acc(void)
{
    uint8_t who = 0;

    read_reg(FXOS8700_WHO_AM_I, &who, 1);

    if (who == FXOS8700_ID) {
        ESP_LOGI(TAG, "FXOS8700 OK");
    } else {
        ESP_LOGE(TAG, "Wrong ID: 0x%02X", who);
    }

    // standby
    write_reg(FXOS8700_CTRL_REG1, 0x00);

    // zakres ±2g
    write_reg(FXOS8700_XYZ_DATA_CFG, 0x00);

    // active + ODR ~100 Hz
    write_reg(FXOS8700_CTRL_REG1, 0x0D);

    ESP_LOGI(TAG, "ACC initialized");
}

// =================================================
// ===================== TASK IMU ===================
// =================================================

void imu_task(void *arg)
{
    uint8_t raw[6];

    while (1)
    {
        if (read_reg(FXOS8700_OUT_X_MSB, raw, 6) == ESP_OK)
        {
            int16_t x = ((int16_t)(raw[0] << 8 | raw[1])) >> 2;
            int16_t y = ((int16_t)(raw[2] << 8 | raw[3])) >> 2;
            int16_t z = ((int16_t)(raw[4] << 8 | raw[5])) >> 2;

            // konwersja do g (±2g)
            float ax = x / 4096.0f;
            float ay = y / 4096.0f;
            float az = z / 4096.0f;

            ESP_LOGI(TAG, "AX: %.2f  AY: %.2f  AZ: %.2f", ax, ay, az);
        }
        else
        {
            ESP_LOGE(TAG, "Read error");
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // ~100 Hz
    }
}

// =================================================
// ===================== MAIN =======================
// =================================================

void app_main(void)
{
    ESP_LOGI(TAG, "System start");

    init_i2c();
    init_acc();

    xTaskCreate(
        imu_task,
        "imu_task",
        4096,
        NULL,
        5,
        NULL
    );
}