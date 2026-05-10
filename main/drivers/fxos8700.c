#include "fxos8700.h"

#include "driver/i2c_master.h"
#include "esp_log.h"

// =================================================
// ===================== CONFIG ====================
// =================================================

#define I2C_SDA_PIN 5
#define I2C_SCL_PIN 4

#define I2C_PORT 0
#define I2C_FREQ_HZ 400000

#define FXOS8700_ADDR 0x1E

#define FXOS8700_WHO_AM_I 0x0D
#define FXOS8700_CTRL_REG1 0x2A
#define FXOS8700_XYZ_DATA_CFG 0x0E
#define FXOS8700_OUT_X_MSB 0x01

#define FXOS8700_ID 0xC7

// =================================================
// ===================== TAG =======================
// =================================================

static const char *TAG = "FXOS8700";

// =================================================
// ===================== HANDLES ===================
// =================================================

static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t imu_handle;

// =================================================
// ===================== WRITE =====================
// =================================================

static esp_err_t write_reg(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};

    return i2c_master_transmit(
        imu_handle,
        data,
        2,
        100
    );
}

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
        imu_handle,
        &reg,
        1,
        data,
        len,
        100
    );
}

// =================================================
// ===================== INIT ======================
// =================================================

esp_err_t fxos8700_init(void)
{
    // =================================================
    // ===================== I2C BUS ===================
    // =================================================

    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = I2C_SCL_PIN,
        .sda_io_num = I2C_SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true
    };

    ESP_ERROR_CHECK(
        i2c_new_master_bus(
            &bus_config,
            &bus_handle
        )
    );

    // =================================================
    // ===================== DEVICE ====================
    // =================================================

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = FXOS8700_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };

    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(
            bus_handle,
            &dev_config,
            &imu_handle
        )
    );

    // =================================================
    // ===================== WHO AM I ==================
    // =================================================

    uint8_t who = 0;

    ESP_ERROR_CHECK(
        read_reg(
            FXOS8700_WHO_AM_I,
            &who,
            1
        )
    );

    if (who != FXOS8700_ID)
    {
        ESP_LOGE(TAG, "Wrong WHO_AM_I: 0x%02X", who);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "FXOS8700 detected");

    // standby
    ESP_ERROR_CHECK(
        write_reg(
            FXOS8700_CTRL_REG1,
            0x00
        )
    );

    // ±2g
    ESP_ERROR_CHECK(
        write_reg(
            FXOS8700_XYZ_DATA_CFG,
            0x00
        )
    );

    // active + 100 Hz
    ESP_ERROR_CHECK(
        write_reg(
            FXOS8700_CTRL_REG1,
            0x0D
        )
    );

    ESP_LOGI(TAG, "Accelerometer initialized");

    return ESP_OK;
}

// =================================================
// ===================== READ ACC ==================
// =================================================

esp_err_t fxos8700_read_accel(
    float *ax,
    float *ay,
    float *az
)
{
    uint8_t raw[6];

    esp_err_t ret = read_reg(
        FXOS8700_OUT_X_MSB,
        raw,
        6
    );

    if (ret != ESP_OK)
    {
        return ret;
    }

    int16_t x =
        ((int16_t)(raw[0] << 8 | raw[1])) >> 2;

    int16_t y =
        ((int16_t)(raw[2] << 8 | raw[3])) >> 2;

    int16_t z =
        ((int16_t)(raw[4] << 8 | raw[5])) >> 2;

    *ax = x / 4096.0f;
    *ay = y / 4096.0f;
    *az = z / 4096.0f;

    return ESP_OK;
}