idf_component_register(SRCS "httptc.c" "mlx90614.c" "nvs_store.c" "wifi_ap.c" "web_server.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash esp_http_server driver esp_wifi)


#idf_component_register(SRCS "httptc.c"
#                   INCLUDE_DIRS ".")
