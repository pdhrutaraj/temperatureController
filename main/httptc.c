#include "wifi_ap.h"
#include "web_server.h"
#include "nvs_store.h"
#include "mlx90614.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"          
#include "nvs_flash.h"            
#include "esp_log.h"
#include <stdbool.h>
#define RELAY_GPIO GPIO_NUM_4
#define OUTPUT_DELAY_GPIO GPIO_NUM_2  

bool relay_on = false;
float current_temp = 0.0;
float target_temp = 0.0;
uint32_t output_delay_ms = 0;
uint32_t output_hold_delay = 500;

void temperature_task(void *pv) {
    float temp;
    while (1) {
        if (mlx90614_read_temp(&temp) == ESP_OK) {
            current_temp = temp;
	    if (current_temp >= target_temp) {
	    	gpio_set_level(RELAY_GPIO,1);
        	vTaskDelay(pdMS_TO_TICKS(output_delay_ms));
	    	gpio_set_level(OUTPUT_DELAY_GPIO,1);
		relay_on = true;
	    } else {
	    	    gpio_set_level(RELAY_GPIO,0);
	    	    gpio_set_level(OUTPUT_DELAY_GPIO,0);
		    relay_on = false;
	    }
	}
        vTaskDelay(pdMS_TO_TICKS(output_hold_delay));
    }
}

void app_main(void) {
    nvs_flash_init();
    
    load_config_from_nvs(&target_temp,&output_delay_ms);
    
    mlx90614_init();
    wifi_init_softap();
    start_webserver();

    gpio_set_direction(RELAY_GPIO, GPIO_MODE_OUTPUT);  
    gpio_set_direction(OUTPUT_DELAY_GPIO, GPIO_MODE_OUTPUT);  
    xTaskCreate(temperature_task, "temp_task", 4096, NULL, 5, NULL);
    
}
