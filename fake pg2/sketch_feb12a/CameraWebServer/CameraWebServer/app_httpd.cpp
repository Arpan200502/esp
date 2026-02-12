#include "esp_http_server.h"
#include "Arduino.h"
#include "app_httpd.h"


#define FLASH_LED_PIN 4

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

esp_err_t led_toggle_handler(httpd_req_t *req) {
  static bool led_on = false;
  led_on = !led_on;
  digitalWrite(FLASH_LED_PIN, led_on ? HIGH : LOW);
  httpd_resp_send(req, "Flash toggled", HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

void setupLedFlash() {
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, LOW);
}

void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_uri_t led_uri = {
    .uri       = "/led",
    .method    = HTTP_GET,
    .handler   = led_toggle_handler,
    .user_ctx  = NULL
  };

  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &led_uri);
  }
}
