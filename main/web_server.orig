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
"<!DOCTYPE html>"
"<html lang=\"en\">"
"<head><meta charset=\"UTF-8\" /><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"/>"
"<title>ESP32 Temp Controller</title>"
"<style>"
"body{font-family:'Segoe UI',sans-serif;margin:0;background:#f3f4f6;color:#111827;display:flex;flex-direction:column;align-items:center;padding:2rem;}"
".container{max-width:400px;width:100%;background:#fff;padding:2rem;border-radius:16px;box-shadow:0 10px 20px rgba(0,0,0,0.1);}"
"h1{font-size:1.5rem;text-align:center;margin-bottom:1rem;}"
".temp-display{text-align:center;font-size:2.5rem;font-weight:bold;margin-bottom:1.5rem;}"
"form{display:flex;flex-direction:column;}"
"label{font-size:0.9rem;margin-bottom:0.25rem;}"
"input[type=number]{padding:0.5rem;font-size:1rem;margin-bottom:1rem;border:1px solid #d1d5db;border-radius:8px;}"
"button{padding:0.75rem;font-size:1rem;background:#3b82f6;color:white;border:none;border-radius:8px;cursor:pointer;transition:background 0.2s;}"
"button:hover{background:#2563eb;}"
".toast{margin-top:1rem;display:none;padding:0.5rem;background:#4ade80;color:#065f46;text-align:center;border-radius:8px;}"
"</style></head>"
"<body><div class=\"container\">"
"<h1>Temperature Controller</h1>"
"<div class=\"temp-display\" id=\"temp\">-- °C</div>"
"<form id=\"temp-form\">"
"<label for=\"target\">Set Target Temperature</label>"
"<input type=\"number\" step=\"0.1\" id=\"target\" name=\"target\" required>"
"<button type=\"submit\">Update</button></form>"
"<div class=\"toast\" id=\"toast\">Temperature Updated!</div></div>"
"<script>"
"function fetchTemp(){fetch(\"/data\").then(res=>res.json()).then(data=>{document.getElementById(\"temp\").innerText=`${data.temp} °C`;});}"
"document.getElementById(\"temp-form\").addEventListener(\"submit\",function(e){e.preventDefault();const target=document.getElementById(\"target\").value;"
"fetch(\"/set\",{method:\"POST\",headers:{\"Content-Type\":\"application/x-www-form-urlencoded\"},body:`target=${target}`})"
".then(()=>{document.getElementById(\"toast\").style.display=\"block\";setTimeout(()=>document.getElementById(\"toast\").style.display=\"none\",2000);});});"
"fetchTemp();setInterval(fetchTemp,3000);"
"</script></body></html>";

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
