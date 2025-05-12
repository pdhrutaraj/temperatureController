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
#define BOTTLE_SENSE_GPIO GPIO_NUM_5

bool relay_on = false;
float current_temp = 0.0;
float target_temp = 0.0;
uint32_t output_delay_ms = 0;
uint32_t output_hold_delay = 500;
int bottle_sense_gpio_level = 1;
bool bottle_sense = false;

void temperature_task(void *pv) {
    float temp;
    while (1) {
        if (mlx90614_read_temp(&temp) == ESP_OK) {
            current_temp = temp;
	    bottle_sense_gpio_level = gpio_get_level(BOTTLE_SENSE_GPIO);
	   if(bottle_sense_gpio_level == 0){
		    bottle_sense = true;
	    }

	    if ((current_temp >= target_temp) && (bottle_sense == true)) {
		bottle_sense = false;
	    	//gpio_set_level(RELAY_GPIO,1);
	    	gpio_set_level(RELAY_GPIO,0);
        	vTaskDelay(pdMS_TO_TICKS(output_delay_ms));
	    	//gpio_set_level(OUTPUT_DELAY_GPIO,1);
	    	gpio_set_level(OUTPUT_DELAY_GPIO,0);
		//relay_on = true;
		relay_on = false;
		
	    } else {
	    	    //gpio_set_level(RELAY_GPIO,0);
		    bottle_sense = false;
	    	    gpio_set_level(RELAY_GPIO,1);
	    	    //gpio_set_level(OUTPUT_DELAY_GPIO,0);
	    	    gpio_set_level(OUTPUT_DELAY_GPIO,1);
		    //relay_on = false;
		    relay_on = true;
		    
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

    // Configure GPIO 5 as input
    gpio_config_t io_conf = {
       .pin_bit_mask = (1ULL << BOTTLE_SENSE_GPIO),
       .mode = GPIO_MODE_INPUT,
       .pull_up_en = GPIO_PULLUP_DISABLE,   // Disable pull-up (enable if needed)
       .pull_down_en = GPIO_PULLDOWN_DISABLE, // Disable pull-down
       .intr_type = GPIO_INTR_DISABLE       // No interrupt
};
gpio_config(&io_conf);

// Read the level (returns 0 or 1)
//int level = gpio_get_level(BOTTLE_SENSE_GPIO);
//printf("GPIO 5 Level: %d\n", level);


    gpio_set_direction(RELAY_GPIO, GPIO_MODE_OUTPUT);  
    gpio_set_direction(OUTPUT_DELAY_GPIO, GPIO_MODE_OUTPUT);  
    xTaskCreate(temperature_task, "temp_task", 4096, NULL, 5, NULL);
    
}
