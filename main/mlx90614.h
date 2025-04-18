// mlx90614.h

#ifndef MLX90614_H
#define MLX90614_H

#include "driver/i2c.h"
#include "esp_err.h"

#define MLX90614_I2C_ADDR       0x5A
#define MLX90614_I2C_NUM        I2C_NUM_0
#define MLX90614_SDA_IO         21
#define MLX90614_SCL_IO         22
#define MLX90614_FREQ_HZ        100000

esp_err_t mlx90614_init(void);
esp_err_t mlx90614_read_temp(float *temp_object);

#endif

