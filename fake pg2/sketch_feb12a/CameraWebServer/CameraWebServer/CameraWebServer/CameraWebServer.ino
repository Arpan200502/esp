#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>

// WiFi credentials
const char* ssid = "Arpan HotSpot";
const char* password = "123456789";

// Camera pin definitions (AI Thinker)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define FLASH_LED_PIN      4

// Web servers
WebServer server(80);        // for control and snapshot
WebServer streamServer(81);  // for MJPEG stream
bool flashEnabled = false;

void startCameraServer();

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  // Flash pin setup
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, LOW);

  // Camera config
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Frame size and buffer count
  if (psramFound()) {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  // Start web servers
  startCameraServer();
}

void loop() {
  server.handleClient();
  streamServer.handleClient();
}

// =============================
// Web server and routes
// =============================
void startCameraServer() {
  // Flash toggle
  server.on("/control", HTTP_GET, []() {
    String var = server.arg("var");
    String val = server.arg("val");
    if (var == "flash") {
      flashEnabled = (val == "1");
      digitalWrite(FLASH_LED_PIN, flashEnabled ? HIGH : LOW);
      server.send(200, "text/plain", flashEnabled ? "Flash ON" : "Flash OFF");
    } else {
      server.send(400, "text/plain", "Bad Request");
    }
  });

  // Snapshot capture
  server.on("/capture", HTTP_GET, []() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      server.send(500, "text/plain", "Camera capture failed");
      return;
    }

    WiFiClient client = server.client();

    // Headers
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: image/jpeg");
    client.println("Content-Disposition: inline; filename=\"capture.jpg\"");
    client.println("Content-Length: " + String(fb->len));
    client.println("Access-Control-Allow-Origin: *");
    client.println();

    // Data
    client.write(fb->buf, fb->len);
    esp_camera_fb_return(fb);
  });

  // MJPEG stream on :81
  streamServer.on("/stream", HTTP_GET, []() {
    WiFiClient client = streamServer.client();
    String response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
    client.print(response);

    while (client.connected()) {
      camera_fb_t *fb = esp_camera_fb_get();
      if (!fb) continue;

      client.printf("--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %d\r\n\r\n", fb->len);
      client.write(fb->buf, fb->len);
      client.print("\r\n");

      esp_camera_fb_return(fb);
      delay(30); // Limit to ~30 FPS
    }
  });

  server.begin();
  streamServer.begin();

  Serial.println("Camera server ready.");
  Serial.println("Stream:    http://<ESP_IP>:81/stream");
  Serial.println("Snapshot:  http://<ESP_IP>/capture");
  Serial.println("Flash Ctrl: http://<ESP_IP>/control?var=flash&val=1");
}
