#include "nvs_flash.h"
#include "nvs.h"
#include<string.h>

void save_config_to_nvs(float target_temp) {
   nvs_handle_t nvs_handle;
   if (nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
        uint32_t temp_bits;
	memcpy(&temp_bits,&target_temp,sizeof(temp_bits));
	nvs_set_u32(nvs_handle,"target_temp",temp_bits);

        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}
void load_config_from_nvs(float *target_temp){
    nvs_handle_t nvs_handle;
    size_t len;
    if (nvs_open("storage", NVS_READONLY, &nvs_handle) == ESP_OK) {
	    uint32_t temp_bits;
	    
	    if (nvs_get_u32(nvs_handle, "target_temp", &temp_bits) == ESP_OK) {
            	memcpy(target_temp, &temp_bits, sizeof(temp_bits));
        	} else {
			*target_temp = 35;
		}
        //len = sizeof(w; nvs_get_str(nvs_handle, "wifi_ssid", wifi_ssid, &len);
        //len = sizeof(wifi_pass); nvs_get_str(nvs_handle, "wifi_pass", wifi_pass, &len);
        nvs_close(nvs_handle);
    }
}
