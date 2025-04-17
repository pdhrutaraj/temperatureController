#include "wifi_ap.h"
#include "web_server.h"
#include "nvs_store.h"
#include "mlx90614.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"          // For gpio_set_level, gpio_set_direction
#include "nvs_flash.h"            // For nvs_flash_init


float current_temp = 0.0;
float target_temp = 30.0;

void temperature_task(void *pv) {
    float temp;
    while (1) {
        if (mlx90614_read_temp(&temp) == ESP_OK) {
            current_temp = temp;
            gpio_set_level(2, current_temp > target_temp);  // GPIO2 controls fan/LED
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void) {
    nvs_flash_init();
    load_target_temp(&target_temp);

    mlx90614_init();
    wifi_init_softap();
    start_webserver();

    gpio_set_direction(2, GPIO_MODE_OUTPUT);  // Set GPIO2 as output
    xTaskCreate(temperature_task, "temp_task", 4096, NULL, 5, NULL);
}
