#ifndef APP_HTTPD_H
#define APP_HTTPD_H

#include "esp_http_server.h"

void startCameraServer();
void setupLedFlash();
esp_err_t led_toggle_handler(httpd_req_t *req);

#endif
