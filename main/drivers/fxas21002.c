#include "fxas21002.h"

#include "driver/i2c_master.h"
#include "esp_log.h"

// =================================================
// ===================== CONFIG ====================
// =================================================

#define FXAS21002_ADDR 0x20

#define FXAS21002_WHO_AM_I 0x0C

#define FXAS21002_ID 0xD7

#define FXAS21002_CTRL_REG0 0x0D
#define FXAS21002_CTRL_REG1 0x13

#define FXAS21002_OUT_X_MSB 0x01

// =================================================
// ===================== TAG =======================
// =================================================

static const char *TAG = "FXAS21002";

// =================================================
// ===================== EXTERN ====================
// =================================================

// używamy TEGO SAMEGO I2C BUS co FXOS

extern i2c_master_bus_handle_t bus_handle;

// =================================================
// ===================== HANDLE ====================
// =================================================

static i2c_master_dev_handle_t gyro_handle;

// =================================================
// ===================== READ ======================
// =================================================

static esp_err_t read_reg(
    uint8_t reg,
    uint8_t *data,
    size_t len
)
{
    return i2c_master_transmit_receive(
        gyro_handle,
        &reg,
        1,
        data,
        len,
        100
    );
}

static esp_err_t write_reg(
    uint8_t reg,
    uint8_t value
)
{
    uint8_t data[2] = {reg, value};

    return i2c_master_transmit(
        gyro_handle,
        data,
        2,
        100
    );
}

// =================================================
// ===================== INIT ======================
// =================================================

esp_err_t fxas21002_init(void)
{
    esp_err_t ret;

    // ================= DEVICE =================

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = FXAS21002_ADDR,
        .scl_speed_hz = 400000,
    };

    ret = i2c_master_bus_add_device(
        bus_handle,
        &dev_config,
        &gyro_handle
    );

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Add device failed");
        return ret;
    }

    // ================= WHO AM I =================

    uint8_t who = 0;

    ret = read_reg(
        FXAS21002_WHO_AM_I,
        &who,
        1
    );

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "WHO_AM_I read failed");
        return ret;
    }

    ESP_LOGI(TAG, "WHO_AM_I = 0x%02X", who);

    if (who != FXAS21002_ID)
    {
        ESP_LOGE(TAG, "Wrong gyro ID");

        return ESP_FAIL;
    }

    ret = write_reg(
    FXAS21002_CTRL_REG1,
    0x00
);

if (ret != ESP_OK)
{
    ESP_LOGE(TAG, "Standby failed");

    return ret;
}

// CTRL_REG0
// FS = 00 = ±250 dps

ret = write_reg(
    FXAS21002_CTRL_REG0,
    0x00
);

if (ret != ESP_OK)
{
    ESP_LOGE(TAG, "Range config failed");

    return ret;
}

// active + 100 Hz

ret = write_reg(
    FXAS21002_CTRL_REG1,
    0x0E
);

if (ret != ESP_OK)
{
    ESP_LOGE(TAG, "Active mode failed");

    return ret;
}

    ESP_LOGI(TAG, "FXAS21002 detected");

    return ESP_OK;
}

esp_err_t fxas21002_read_gyro(
    float *gx,
    float *gy,
    float *gz
)
{
    uint8_t raw[6];

    esp_err_t ret = read_reg(
        FXAS21002_OUT_X_MSB,
        raw,
        6
    );

    if (ret != ESP_OK)
    {
        return ret;
    }

    int16_t x =
        (int16_t)(
            raw[0] << 8 | raw[1]
        );

    int16_t y =
        (int16_t)(
            raw[2] << 8 | raw[3]
        );

    int16_t z =
        (int16_t)(
            raw[4] << 8 | raw[5]
        );

    // ±250 dps
    // 1 dps = 80 LSB

    *gx = x / 80.0f;
    *gy = y / 80.0f;
    *gz = z / 80.0f;

    return ESP_OK;
}