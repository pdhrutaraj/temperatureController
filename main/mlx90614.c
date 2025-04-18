#include "mlx90614.h"
#include "esp_log.h"
#include "freertos/task.h"

#define MLX90614_REGISTER_TOBJ1 0x07
static const char *TAG = "MLX90614";

esp_err_t mlx90614_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = MLX90614_SDA_IO,
        .scl_io_num = MLX90614_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = MLX90614_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(MLX90614_I2C_NUM, &conf));
    return i2c_driver_install(MLX90614_I2C_NUM, conf.mode, 0, 0, 0);
}

esp_err_t mlx90614_read_temp(float *temp_object) {
    uint8_t cmd = MLX90614_REGISTER_TOBJ1;
    uint8_t data[3];

    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    i2c_master_start(handle);
    i2c_master_write_byte(handle, (MLX90614_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(handle, cmd, true);
    i2c_master_start(handle);
    i2c_master_write_byte(handle, (MLX90614_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(handle, data, 3, I2C_MASTER_LAST_NACK);
    i2c_master_stop(handle);

    esp_err_t err = i2c_master_cmd_begin(MLX90614_I2C_NUM, handle, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(handle);

    if (err != ESP_OK) return err;

    uint16_t raw = data[0] | (data[1] << 8);
    *temp_object = (raw * 0.02) - 273.15;  // Convert to Celsius
    return ESP_OK;
}

