#ifndef NVS_STORE_H
#define NVS_STORE_H

#include "esp_err.h"

esp_err_t save_target_temp(float temp);
esp_err_t load_target_temp(float *temp);

#endif