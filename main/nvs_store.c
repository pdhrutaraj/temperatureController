#include "nvs_flash.h"
#include "nvs.h"

esp_err_t save_target_temp(float temp) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    err = nvs_set_blob(handle, "target", &temp, sizeof(temp));
    if (err == ESP_OK) err = nvs_commit(handle);
    nvs_close(handle);
    return err;
}

esp_err_t load_target_temp(float *temp) {
    nvs_handle_t handle;
    size_t required_size = sizeof(float);
    esp_err_t err = nvs_open("storage", NVS_READONLY, &handle);
    if (err != ESP_OK) return err;

    err = nvs_get_blob(handle, "target", temp, &required_size);
    nvs_close(handle);
    return err;
}