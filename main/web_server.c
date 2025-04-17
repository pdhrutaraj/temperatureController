#include "web_server.h"
#include "esp_http_server.h"
#include "nvs_store.h"
#include "esp_macros.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

extern float current_temp;
extern float target_temp;

static const char *HTML_INDEX =
"<!DOCTYPE html><html><head><title>Temp Control</title>"
"<script>function setTemp() {"
"fetch('/set', {method:'POST', body:'target='+document.getElementById('t').value});"
"}"
"setInterval(()=>fetch('/data').then(r=>r.json()).then(d=>{"
"document.getElementById('c').innerText=d.temp;"
"document.getElementById('s').innerText=d.fan;"
"}), 2000);</script></head>"
"<body><h2>ESP32 Temp Control</h2>"
"<p>Current Temp: <span id='c'>--</span> &deg;C</p>"
"<p>Fan Status: <span id='s'>--</span></p>"
"<input id='t' type='number' step='0.1'><button onclick='setTemp()'>Set Target</button>"
"</body></html>";

esp_err_t index_get(httpd_req_t *req) {
    httpd_resp_send(req, HTML_INDEX, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t temp_post(httpd_req_t *req) {
    char buf[100];
    httpd_req_recv(req, buf, MIN(req->content_len, sizeof(buf)));
    char *target = strstr(buf, "target=");
    if (target) {
        target_temp = atof(target + 7);
        save_target_temp(target_temp);
    }
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

esp_err_t data_get(httpd_req_t *req) {
    char resp[100];
    snprintf(resp, sizeof(resp),
             "{\"temp\":%.2f,\"fan\":\"%s\"}",
             current_temp, current_temp > target_temp ? "ON" : "OFF");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &(httpd_uri_t){"/", HTTP_GET, index_get});
        httpd_register_uri_handler(server, &(httpd_uri_t){"/set", HTTP_POST, temp_post});
        httpd_register_uri_handler(server, &(httpd_uri_t){"/data", HTTP_GET, data_get});
    }
    return server;
}
