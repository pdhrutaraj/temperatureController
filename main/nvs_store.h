#ifndef NVS_STORE_H
#define NVS_STORE_H

#include "esp_err.h"

void save_config_to_nvs(float temp,int output_delay_ms);
void load_config_from_nvs(float *temp,int *output_delay_ms);

#endif
